import { CompilerService } from '../src/services/compilerService.js';
import { DebuggerService } from '../src/services/debuggerService.js';

async function verify() {
    console.log('🚀 Starting Backend Verification...');

    const compilerService = new CompilerService();
    const debuggerService = new DebuggerService();

    const sourceCode = `
    int main() {
      int x = 10;
      int y = 20;
      int z = x + y;
      return z;
    }
  `;

    console.log('1️⃣ Compiling code...');
    const compileResult = await compilerService.compileCode(sourceCode);

    if (!compileResult.success) {
        console.error('❌ Compilation failed:', compileResult.error);
        if (compileResult.error && compileResult.error.stack) {
            console.error(compileResult.error.stack);
        }
        return;
    }
    console.log('✅ Compilation successful');
    console.log('📄 Assembly length:', compileResult.assembly.length);

    console.log('2️⃣ Starting execution...');
    const startResult = await debuggerService.startExecution(compileResult.assembly);
    if (!startResult.success) {
        console.error('❌ Start execution failed:', startResult.error);
        return;
    }
    console.log('✅ Execution started');
    console.log('   RIP:', startResult.state.registers.rip);

    console.log('3️⃣ Stepping forward...');
    for (let i = 0; i < 10; i++) {
        const stepResult = await debuggerService.stepForward();
        if (!stepResult.success) {
            if (stepResult.error === 'Execution finished') {
                console.log('🏁 Execution finished normally');
                break;
            }
            console.error(`❌ Step ${i + 1} failed:`, stepResult.error);
            break;
        }
        console.log(`   Step ${i + 1}: RIP=${stepResult.state.registers.rip}, RAX=${stepResult.state.registers.rax}, RSP=${stepResult.state.registers.rsp}`);
    }

    console.log('✅ Verification complete');
}

verify();
