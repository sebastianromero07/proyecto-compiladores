export class DebuggerService {
  constructor() {
    this.reset();
  }

  reset() {
    this.executionState = {
      registers: {
        rax: 0, rbx: 0, rcx: 0, rdx: 0,
        rsi: 0, rdi: 0, rsp: 1000, rbp: 1000,
        rip: 0, r8: 0, r9: 0, r10: 0, r11: 0, r12: 0, r13: 0, r14: 0, r15: 0
      },
      stack: [], // Array of { offset, address, value, isRsp, isRbp }
      memory: new Map(), // Address -> Value
      currentLine: 0,
      stepCount: 0,
      finished: false,
      modifiedRegisters: [] // Track which registers changed in last step
    };

    this.instructions = [];
    this.labels = new Map();
    this.history = [];
    this.previousRegisters = null;
  }

  async startExecution(sourceCode) {
    try {
      this.reset();
      this.parseAssembly(sourceCode);

      // Find main entry point
      if (this.labels.has('main')) {
        this.executionState.registers.rip = this.labels.get('main');
      }

      // Save initial register state
      this.previousRegisters = { ...this.executionState.registers };
      this.updateStackView();

      return {
        success: true,
        state: this.formatState(),
        message: 'Execution started'
      };
    } catch (error) {
      console.error('Start execution error:', error);
      return {
        success: false,
        error: error.message
      };
    }
  }

  parseAssembly(code) {
    const lines = code.split('\n');
    let addr = 0;

    lines.forEach((line, index) => {
      const cleanLine = line.trim().replace(/#.*$/, '').trim(); // Remove comments
      if (!cleanLine) return;

      if (cleanLine.endsWith(':')) {
        const label = cleanLine.slice(0, -1);
        this.labels.set(label, addr);
      } else if (!cleanLine.startsWith('.')) { // Ignore directives for now
        this.instructions.push({
          text: cleanLine,
          originalLine: index + 1,
          addr: addr
        });
        addr++;
      }
    });
  }

  async stepForward() {
    if (this.executionState.finished || this.executionState.registers.rip >= this.instructions.length) {
      return { success: false, error: 'Execution finished' };
    }

    // Save history
    this.history.push(JSON.parse(JSON.stringify(this.executionState)));

    const inst = this.instructions[this.executionState.registers.rip];
    if (!inst) {
      this.executionState.finished = true;
      return { success: false, error: 'Invalid instruction pointer' };
    }

    try {
      // Save previous register state
      this.previousRegisters = { ...this.executionState.registers };

      this.executeInstruction(inst.text);
      this.executionState.registers.rip++;
      this.executionState.stepCount++;
      this.executionState.currentLine = inst.originalLine;

      // Detect which registers changed
      this.executionState.modifiedRegisters = this.detectModifiedRegisters();

      this.updateStackView();

      return {
        success: true,
        state: this.formatState(),
        direction: 'forward'
      };
    } catch (error) {
      return { success: false, error: error.message };
    }
  }

  async stepBack() {
    if (this.history.length === 0) {
      return { success: false, error: 'No history available' };
    }

    const prevState = this.history.pop();
    this.executionState = prevState;

    // Restore memory map from stack array (simplified)
    this.executionState.memory = new Map();
    this.executionState.stack.forEach(item => {
      const addr = parseInt(item.address, 16);
      const val = parseInt(item.value, 16);
      if (!isNaN(addr) && !isNaN(val)) {
        this.executionState.memory.set(addr, val);
      }
    });

    // Update previous registers for highlighting
    if (this.history.length > 0) {
      this.previousRegisters = { ...this.history[this.history.length - 1].registers };
    } else {
      this.previousRegisters = null;
    }

    this.executionState.modifiedRegisters = this.detectModifiedRegisters();

    return {
      success: true,
      state: this.formatState(),
      direction: 'backward'
    };
  }

  detectModifiedRegisters() {
    if (!this.previousRegisters) return [];

    const modified = [];
    for (const reg in this.executionState.registers) {
      if (this.previousRegisters[reg] !== this.executionState.registers[reg]) {
        modified.push(reg);
      }
    }
    return modified;
  }

  executeInstruction(text) {
    const parts = text.split(/\s+/);
    const opcode = parts[0];
    const args = parts.slice(1).join('').split(',');

    switch (opcode) {
      case 'movq':
        this.setVal(args[1], this.getVal(args[0]));
        break;
      case 'addq':
        this.setVal(args[1], this.getVal(args[1]) + this.getVal(args[0]));
        break;
      case 'subq':
        this.setVal(args[1], this.getVal(args[1]) - this.getVal(args[0]));
        break;
      case 'imulq':
        this.setVal(args[1], this.getVal(args[1]) * this.getVal(args[0]));
        break;
      case 'pushq':
        this.executionState.registers.rsp -= 8;
        this.writeMem(this.executionState.registers.rsp, this.getVal(args[0]));
        break;
      case 'popq':
        this.setVal(args[0], this.readMem(this.executionState.registers.rsp));
        this.executionState.registers.rsp += 8;
        break;
      case 'call':
        this.executionState.registers.rsp -= 8;
        // Mock return address (just next instruction)
        this.writeMem(this.executionState.registers.rsp, this.executionState.registers.rip + 1);
        if (this.labels.has(args[0])) {
          this.executionState.registers.rip = this.labels.get(args[0]) - 1; // -1 because rip++ happens after
        }
        break;
      case 'ret':
        const retAddr = this.readMem(this.executionState.registers.rsp);
        this.executionState.registers.rsp += 8;
        this.executionState.registers.rip = retAddr - 1;
        break;
      // Add more instructions as needed (cmp, jmp, etc.)
    }
  }

  getVal(arg) {
    arg = arg.trim();
    if (arg.startsWith('$')) return parseInt(arg.substring(1));
    if (arg.startsWith('%')) return this.executionState.registers[arg.substring(1)];
    if (arg.match(/^-?\d+\(%[a-z]+\)$/)) { // -8(%rbp)
      const match = arg.match(/^(-?\d+)\(%([a-z]+)\)$/);
      const offset = parseInt(match[1]);
      const reg = match[2];
      const addr = this.executionState.registers[reg] + offset;
      return this.readMem(addr);
    }
    return 0;
  }

  setVal(arg, val) {
    arg = arg.trim();
    if (arg.startsWith('%')) {
      this.executionState.registers[arg.substring(1)] = val;
    } else if (arg.match(/^-?\d+\(%[a-z]+\)$/)) {
      const match = arg.match(/^(-?\d+)\(%([a-z]+)\)$/);
      const offset = parseInt(match[1]);
      const reg = match[2];
      const addr = this.executionState.registers[reg] + offset;
      this.writeMem(addr, val);
    }
  }

  readMem(addr) {
    return this.executionState.memory.get(addr) || 0;
  }

  writeMem(addr, val) {
    this.executionState.memory.set(addr, val);
  }

  updateStackView() {
    const stack = [];
    const rsp = this.executionState.registers.rsp;
    const rbp = this.executionState.registers.rbp;

    // Show stack from RBP down to RSP (and a bit beyond)
    for (let addr = rbp + 16; addr >= rsp - 16; addr -= 8) {
      const offset = addr - rbp; // Calculate offset from RBP
      const value = this.readMem(addr);

      stack.push({
        offset: offset,
        address: addr.toString(16),
        value: value.toString(16),
        isRsp: addr === rsp,
        isRbp: addr === rbp,
        hasValue: this.executionState.memory.has(addr)
      });
    }

    this.executionState.stack = stack;
  }

  formatState() {
    return {
      registers: this.executionState.registers,
      stack: this.executionState.stack,
      currentLine: this.executionState.currentLine,
      stepCount: this.executionState.stepCount,
      modifiedRegisters: this.executionState.modifiedRegisters || []
    };
  }

  getCurrentState() {
    return {
      success: true,
      state: this.formatState()
    };
  }
}