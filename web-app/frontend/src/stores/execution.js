import { reactive } from 'vue';
import { AssemblySimulator } from '../utils/AssemblySimulator';

export const executionStore = reactive({
  simulator: null,
  currentLine: 0,
  registers: {},
  stack: [],
  isFinished: false,

  init(assemblyCode) {
    this.simulator = new AssemblySimulator(assemblyCode);
    this.updateState();
  },

  step() {
    if (this.simulator && !this.simulator.finished) {
      this.simulator.step();
      this.updateState();
    }
  },

  updateState() {
    const state = this.simulator.getCurrentState();
    this.currentLine = state.line;
    this.registers = state.registers;
    this.stack = state.stack;
    this.isFinished = this.simulator.finished;
  },

  reset() {
    this.simulator = null;
    this.currentLine = 0;
    this.stack = [];
  }
});

