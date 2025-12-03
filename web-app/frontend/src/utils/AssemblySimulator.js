export class AssemblySimulator {
  constructor(assemblyCode) {
    // Limpiar caracteres de Windows (\r) y dividir líneas
    this.lines = assemblyCode.replace(/\r/g, '').split('\n');
    this.labels = this.parseLabels();
    
    console.log('=== SIMULADOR INICIALIZADO ===');
    console.log('Total de líneas:', this.lines.length);
    console.log('Labels encontradas:', this.labels);
    
    // Estado inicial - buscar main
    this.ip = this.labels['main'] !== undefined ? this.labels['main'] : 0;
    console.log('IP inicial (main):', this.ip);
    
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
      
      // Detectar etiquetas: cualquier cosa que termine en ":" y no sea una directiva .string
      // Ejemplos: "suma:", ".end_suma:", "main:"
      if (cleanLine.endsWith(':') && !cleanLine.includes('.string')) {
        // Extraer el nombre de la etiqueta (sin los dos puntos)
        const labelName = cleanLine.slice(0, -1).trim();
        map[labelName] = index;
        console.log(`Label: '${labelName}' -> línea ${index}`);
      }
    });
    return map;
  }

  getOperandValue(operand) {
    operand = operand.trim();
    
    if (operand.startsWith('$')) {
      return parseInt(operand.substring(1));
    }
    
    if (operand.startsWith('%')) {
      const reg = operand.substring(1).toLowerCase();
      const regMap = { 'eax': 'rax', 'ebx': 'rbx', 'ecx': 'rcx', 'edx': 'rdx' };
      const actualReg = regMap[reg] || reg;
      return this.registers[actualReg] || 0;
    }
    
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
    
    if (operand.startsWith('%')) {
      const reg = operand.substring(1).toLowerCase();
      const regMap = { 'eax': 'rax', 'ebx': 'rbx', 'ecx': 'rcx', 'edx': 'rdx' };
      const actualReg = regMap[reg] || reg;
      if (this.registers[actualReg] !== undefined) {
        this.registers[actualReg] = value;
      }
      return;
    }
    
    const memMatch = operand.match(/^(-?\d*)\(%([a-z0-9]+)\)$/);
    if (memMatch) {
      const offset = memMatch[1] ? parseInt(memMatch[1]) : 0;
      const baseReg = memMatch[2];
      const addr = (this.registers[baseReg] || 0) + offset;
      this.memory[addr] = value;
      this.updateStackView();
    }
  }

  updateStackView() {
    this.stack = [];
    const rbp = this.registers.rbp;
    const rsp = this.registers.rsp;
    
    for (let addr = rbp + 16; addr >= rsp - 48; addr -= 8) {
      const value = this.memory[addr] || 0;
      
      this.stack.push({
        address: addr,
        offset: addr - rbp,
        value: value,
        desc: addr === rbp ? 'RBP' : (addr === rsp ? 'RSP' : ''),
        isRbp: addr === rbp,
        isRsp: addr === rsp
      });
    }
  }

  step() {
    if (this.ip >= this.lines.length || this.finished) return;

    const currentIp = this.ip;
    let line = this.lines[this.ip].trim();
    
    console.log(`[STEP] Línea ${this.ip}: "${line}"`);
    
    // Saltar líneas vacías, etiquetas, o directivas
    if (line === '' || 
        line.endsWith(':') || 
        (line.startsWith('.') && !line.includes(':'))) {
      this.ip++;
      if (this.ip < this.lines.length && !this.finished) {
        this.step();
      }
      return;
    }

    // === CALL ===
    if (line.startsWith('call')) {
      const parts = line.split(/\s+/);
      let target = parts[1];
      
      if (target) {
        target = target.replace('@PLT', '').trim();
        
        console.log(`[CALL] Objetivo: '${target}'`);
        console.log(`[CALL] Labels disponibles:`, Object.keys(this.labels));
        console.log(`[CALL] ¿Existe '${target}'?:`, this.labels[target] !== undefined);
        
        // Push dirección de retorno
        const returnAddress = this.ip + 1;
        this.registers.rsp -= 8;
        this.memory[this.registers.rsp] = returnAddress;
        this.callStack.push(returnAddress);
        
        // Saltar a la función
        if (this.labels[target] !== undefined) {
          const targetLine = this.labels[target];
          console.log(`[CALL] Saltando de línea ${this.ip} a línea ${targetLine}`);
          this.ip = targetLine;
        } else {
          console.warn(`[CALL] Función externa '${target}', continuando...`);
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
        console.log(`[RET] Volviendo a línea ${returnIp}`);
        this.ip = returnIp;
        this.registers.rsp += 8;
      } else {
        console.log(`[RET] Fin del programa`);
        this.finished = true;
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
      this.ip++;
      this.updateStackView();
      this.updateRip();
      return;
    }

    // === MOV (movq, movl, movsd) ===
    if (line.match(/^mov[qlsd]+/)) {
      const parts = line.replace(/^mov[qlsd]+\s+/, '').split(',');
      if (parts.length === 2) {
        const src = parts[0].trim();
        const dst = parts[1].trim();
        const value = this.getOperandValue(src);
        this.setOperandValue(dst, value);
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
      }
      this.ip++;
      this.updateRip();
      return;
    }

    // === JMP ===
    if (line.startsWith('jmp')) {
      const target = line.split(/\s+/)[1];
      if (this.labels[target] !== undefined) {
        console.log(`[JMP] Saltando a '${target}' (línea ${this.labels[target]})`);
        this.ip = this.labels[target];
      } else {
        this.ip++;
      }
      this.updateRip();
      return;
    }

    // === Otras instrucciones (leaq, cmpq, etc.) ===
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