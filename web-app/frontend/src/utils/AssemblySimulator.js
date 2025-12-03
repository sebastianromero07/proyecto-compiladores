export class AssemblySimulator {
  constructor(assemblyCode) {
    this.lines = assemblyCode.replace(/\r/g, '').split('\n');
    this.labels = this.parseLabels();
    
    console.log('=== SIMULADOR INICIALIZADO ===');
    console.log('Labels encontradas:', this.labels);
    
    this.ip = this.labels['main'] !== undefined ? this.labels['main'] : 0;
    
    this.registers = {
      rax: 0, rbx: 0, rcx: 0, rdx: 0,
      rsi: 0, rdi: 0, rbp: 1000, rsp: 1000,
      rip: this.ip, r8: 0
    };
    
    this.memory = {};
    this.stack = [];
    this.callStack = [];
    this.finished = false;
    
    this.updateStackView();
  }

  parseLabels() {
    const map = {};
    this.lines.forEach((line, index) => {
      const cleanLine = line.trim();
      if (cleanLine.endsWith(':') && !cleanLine.includes('.string')) {
        const labelName = cleanLine.slice(0, -1).trim();
        map[labelName] = index;
      }
    });
    return map;
  }

  getOperandValue(operand) {
    operand = operand.trim();
    
    // Inmediato: $123
    if (operand.startsWith('$')) {
      const val = operand.substring(1);
      return parseFloat(val) || parseInt(val) || 0;
    }
    
    // Registro: %rax, %eax, etc.
    if (operand.startsWith('%')) {
      const reg = operand.substring(1).toLowerCase();
      const regMap = { 'eax': 'rax', 'ebx': 'rbx', 'ecx': 'rcx', 'edx': 'rdx' };
      const actualReg = regMap[reg] || reg;
      return this.registers[actualReg] || 0;
    }
    
    // Memoria indirecta simple: (%rcx) o (%rsp)
    const simpleMemMatch = operand.match(/^\(%(r[a-z0-9]+)\)$/);
    if (simpleMemMatch) {
      const baseReg = simpleMemMatch[1];
      const addr = this.registers[baseReg] || 0;
      return this.memory[addr] || 0;
    }
    
    // Memoria con offset: -8(%rbp) o 0(%rsp)
    const memMatch = operand.match(/^(-?\d*)\(%([a-z0-9]+)\)$/);
    if (memMatch) {
      const offset = memMatch[1] ? parseInt(memMatch[1]) : 0;
      const baseReg = memMatch[2];
      const addr = (this.registers[baseReg] || 0) + offset;
      return this.memory[addr] || 0;
    }
    
    return 0;
  }

    setOperandValue(operand, value) {
    operand = operand.trim();
    
    // Registro
    if (operand.startsWith('%')) {
      const reg = operand.substring(1).toLowerCase();
      const regMap = { 'eax': 'rax', 'ebx': 'rbx', 'ecx': 'rcx', 'edx': 'rdx' };
      const actualReg = regMap[reg] || reg;
      if (this.registers[actualReg] !== undefined) {
        this.registers[actualReg] = value;
      }
      return;
    }
    
    // Memoria indirecta simple: (%rcx)
    const simpleMemMatch = operand.match(/^\(%(r[a-z0-9]+)\)$/);
    if (simpleMemMatch) {
      const baseReg = simpleMemMatch[1];
      const addr = this.registers[baseReg] || 0;
      this.memory[addr] = value;  // <-- Esto marca que addr fue escrito
      console.log(`[MEM] Escribiendo ${value} en dirección ${addr}`);
      this.updateStackView();
      return;
    }
    
    // Memoria con offset: -8(%rbp)
    const memMatch = operand.match(/^(-?\d*)\(%([a-z0-9]+)\)$/);
    if (memMatch) {
      const offset = memMatch[1] ? parseInt(memMatch[1]) : 0;
      const baseReg = memMatch[2];
      const addr = (this.registers[baseReg] || 0) + offset;
      this.memory[addr] = value;  // <-- Esto marca que addr fue escrito
      console.log(`[MEM] Escribiendo ${value} en dirección ${addr} (${baseReg}${offset >= 0 ? '+' : ''}${offset})`);
      this.updateStackView();
      return;
    }
  }

      updateStackView() {
    this.stack = [];
    const rbp = this.registers.rbp;
    const rsp = this.registers.rsp;
    
    // Mostrar desde rbp+16 hasta rsp-16
    const startAddr = rbp + 16;
    const endAddr = Math.min(rsp - 16, rbp - 64);
    
    for (let addr = startAddr; addr >= endAddr; addr -= 8) {
      // IMPORTANTE: Verificar si la dirección fue escrita (existe en memoria)
      const hasBeenWritten = Object.prototype.hasOwnProperty.call(this.memory, addr);
      const value = this.memory[addr] !== undefined ? this.memory[addr] : 0;
      
      this.stack.push({
        address: addr,
        offset: addr - rbp,
        value: value,
        isRbp: addr === rbp,
        isRsp: addr === rsp,
        // Mostrar el valor si fue escrito EN CUALQUIER MOMENTO, incluso si es 0
        hasValue: hasBeenWritten
      });
    }
  }

  step() {
    if (this.ip >= this.lines.length || this.finished) return;

    let line = this.lines[this.ip].trim();
    
    console.log(`[STEP ${this.ip}] "${line}"`);
    
    // Saltar líneas vacías, etiquetas, directivas
    if (line === '' || line.endsWith(':') || 
        (line.startsWith('.') && !line.includes(':'))) {
      this.ip++;
      if (this.ip < this.lines.length && !this.finished) {
        this.step();
      }
      return;
    }

    // === LEAQ (Load Effective Address) ===
    if (line.startsWith('leaq')) {
      const match = line.match(/leaq\s+(.+),\s*(%\w+)/);
      if (match) {
        const src = match[1].trim();
        const dst = match[2].trim();
        
        // Calcular dirección efectiva
        const memMatch = src.match(/^(-?\d*)\(%([a-z0-9]+)\)$/);
        if (memMatch) {
          const offset = memMatch[1] ? parseInt(memMatch[1]) : 0;
          const baseReg = memMatch[2];
          const addr = (this.registers[baseReg] || 0) + offset;
          this.setOperandValue(dst, addr);
          console.log(`[LEAQ] ${dst} = ${addr} (${baseReg}${offset >= 0 ? '+' : ''}${offset})`);
        }
      }
      this.ip++;
      this.updateRip();
      return;
    }

    // === CALL ===
    if (line.startsWith('call')) {
      const parts = line.split(/\s+/);
      let target = parts[1];
      
      if (target) {
        target = target.replace('@PLT', '').trim();
        
        console.log(`[CALL] -> ${target}`);
        
        const returnAddress = this.ip + 1;
        this.registers.rsp -= 8;
        this.memory[this.registers.rsp] = returnAddress;
        this.callStack.push(returnAddress);
        
        if (this.labels[target] !== undefined) {
          this.ip = this.labels[target];
        } else {
          console.warn(`[CALL] Función externa: ${target}`);
          this.ip++;
        }
        
        this.updateStackView();
        this.updateRip();
        return;
      }
    }

    // === RET ===
    if (line === 'ret') {
      if (this.callStack.length > 0) {
        const returnIp = this.callStack.pop();
        this.ip = returnIp;
        this.registers.rsp += 8;
        console.log(`[RET] -> línea ${returnIp}`);
      } else {
        this.finished = true;
        console.log(`[RET] Fin del programa`);
      }
      this.updateStackView();
      this.updateRip();
      return;
    }

    // === LEAVE ===
    if (line === 'leave') {
      this.registers.rsp = this.registers.rbp;
      this.registers.rbp = this.memory[this.registers.rsp] || 0;
      this.registers.rsp += 8;
      this.ip++;
      this.updateStackView();
      this.updateRip();
      return;
    }

    // === PUSHQ ===
    if (line.startsWith('pushq')) {
      const operand = line.replace('pushq', '').trim();
      const value = this.getOperandValue(operand);
      this.registers.rsp -= 8;
      this.memory[this.registers.rsp] = value;
      console.log(`[PUSH] ${operand} = ${value} -> RSP=${this.registers.rsp}`);
      this.ip++;
      this.updateStackView();
      this.updateRip();
      return;
    }

    // === POPQ ===
    if (line.startsWith('popq')) {
      const operand = line.replace('popq', '').trim();
      const value = this.memory[this.registers.rsp] || 0;
      this.setOperandValue(operand, value);
      this.registers.rsp += 8;
      console.log(`[POP] ${operand} = ${value}, RSP=${this.registers.rsp}`);
      this.ip++;
      this.updateStackView();
      this.updateRip();
      return;
    }

    // === MOV (movq, movl, movsd) ===
    if (line.match(/^mov[qlsd]+/)) {
      const rest = line.replace(/^mov[qlsd]+\s+/, '');
      const parts = rest.split(',');
      if (parts.length === 2) {
        const src = parts[0].trim();
        const dst = parts[1].trim();
        const value = this.getOperandValue(src);
        this.setOperandValue(dst, value);
        console.log(`[MOV] ${dst} = ${value}`);
      }
      this.ip++;
      this.updateStackView();
      this.updateRip();
      return;
    }

    // === SUBQ ===
    if (line.startsWith('subq')) {
      const parts = line.replace('subq', '').split(',');
      if (parts.length === 2) {
        const src = this.getOperandValue(parts[0].trim());
        const dst = parts[1].trim();
        const currentVal = this.getOperandValue(dst);
        this.setOperandValue(dst, currentVal - src);
        console.log(`[SUB] ${dst} = ${currentVal} - ${src} = ${currentVal - src}`);
      }
      this.ip++;
      this.updateStackView();
      this.updateRip();
      return;
    }

    // === ADDQ ===
    if (line.startsWith('addq')) {
      const parts = line.replace('addq', '').split(',');
      if (parts.length === 2) {
        const src = this.getOperandValue(parts[0].trim());
        const dst = parts[1].trim();
        const currentVal = this.getOperandValue(dst);
        this.setOperandValue(dst, currentVal + src);
        console.log(`[ADD] ${dst} = ${currentVal} + ${src} = ${currentVal + src}`);
      }
      this.ip++;
      this.updateStackView();
      this.updateRip();
      return;
    }

    // === JMP ===
    if (line.startsWith('jmp')) {
      const target = line.split(/\s+/)[1];
      if (this.labels[target] !== undefined) {
        this.ip = this.labels[target];
        console.log(`[JMP] -> ${target} (línea ${this.ip})`);
      } else {
        this.ip++;
      }
      this.updateRip();
      return;
    }

    // === Instrucciones no manejadas ===
    console.log(`[SKIP] Instrucción no manejada: ${line}`);
    this.ip++;
    this.updateStackView();
    this.updateRip();
  }

  updateRip() {
    this.registers.rip = this.ip;
  }

  getCurrentState() {
    return {
      line: this.ip,
      registers: { ...this.registers },
      stack: [...this.stack],
      memory: { ...this.memory }
    };
  }
}