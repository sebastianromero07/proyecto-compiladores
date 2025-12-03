<template>
  <div class="h-screen w-screen overflow-hidden bg-black text-gray-100 flex flex-col">
    <!-- Notificación -->
    <div 
      v-if="notification.show" 
      :class="[
        'fixed top-4 right-4 px-4 py-2 rounded-lg z-50 transition-opacity',
        notification.type === 'success' ? 'bg-green-600' : 
        notification.type === 'error' ? 'bg-red-600' : 'bg-blue-600'
      ]"
    >
      {{ notification.message }}
    </div>

    <!-- Control Panel -->
    <div class="flex-shrink-0 bg-gray-900 border-b border-cyan-500/30 p-2">
      <div class="flex items-center gap-4">
        <button 
          @click="handleStepBackward"
          :disabled="stepCount === 0"
          class="px-4 py-2 bg-gray-700 hover:bg-gray-600 disabled:opacity-50 rounded"
        >
          ← BACK
        </button>
        <button 
          @click="handleStepForward"
          :disabled="!canStep"
          class="px-4 py-2 bg-cyan-600 hover:bg-cyan-500 disabled:opacity-50 rounded"
        >
          FORWARD →
        </button>
        <button 
          @click="handleCompile"
          :disabled="isCompiling"
          class="px-4 py-2 bg-purple-600 hover:bg-purple-500 disabled:opacity-50 rounded"
        >
          {{ isCompiling ? 'COMPILING...' : 'COMPILE' }}
        </button>
        <button 
          @click="handleReset"
          class="px-4 py-2 bg-gray-700 hover:bg-gray-600 rounded"
        >
          RESET
        </button>
        
        <span class="text-sm text-gray-400">
          STEP: {{ stepCount }} | LINE: {{ executionState.currentLine }} | STATUS: {{ compileStatus }}
        </span>
      </div>
    </div>

      <!-- Main Content -->
    <div class="flex flex-1 overflow-hidden">
      <!-- Left Panel: Code Editor + Assembly -->
      <div :style="{ width: leftWidth + '%' }" class="flex flex-col overflow-hidden">
        <!-- Source Code - SIN título duplicado -->
        <div class="flex-1 flex flex-col overflow-hidden border-b border-cyan-500/30">
          <div class="flex-1 overflow-hidden">
            <CodeEditor v-model="sourceCode" />
          </div>
        </div>
        
        <!-- Assembly Code - SIN título duplicado -->
        <div class="flex-1 flex flex-col overflow-hidden">
          <div class="flex-1 overflow-y-auto">
            <AssemblyView 
              :code="compiledCode.assembly" 
              :currentLine="executionState.currentLine"
            />
          </div>
        </div>
      </div>


      <!-- Resize Handle -->
      <div 
        class="w-1 bg-cyan-500/20 hover:bg-cyan-500 cursor-col-resize transition-colors flex-shrink-0"
        @mousedown="startResize('middle', $event)"
      ></div>

      <!-- Right Panel: Registers + Stack -->
      <div class="flex-1 flex flex-col overflow-hidden bg-black">
        <!-- Registers -->
        <div :style="{ height: topRightHeight + '%' }" class="flex-shrink-0 overflow-hidden">
          <RegisterView 
            :registers="executionState.registers"
            :modifiedRegs="executionState.modifiedRegisters"
          />
        </div>

        <!-- Vertical Resize Handle -->
        <div 
          class="h-1 bg-cyan-500/20 hover:bg-cyan-500 cursor-row-resize transition-colors flex-shrink-0"
          @mousedown="startResize('vertical', $event)"
        ></div>

        <!-- Stack Memory -->
        <div class="flex-1 overflow-hidden">
          <StackView :stack="executionState.stack" />
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import CodeEditor from './components/CodeEditor.vue'
import RegisterView from './components/RegisterView.vue'
import StackView from './components/StackView.vue'
import AssemblyView from './components/AssemblyView.vue'
import { compilerStore } from './stores/compiler'
import { AssemblySimulator } from './utils/AssemblySimulator'

export default {
  name: 'App',
  components: {
    CodeEditor,
    RegisterView,
    StackView,
    AssemblyView
  },
  
  data() {
    return {
      sourceCode: `int suma(int a, int b) {
    int resultado = 0;
    resultado = a + b;
    return resultado;
}

int main() {
    int x = 0;
    int y = 0;
    int z = 0;
    x = 2;
    y = 3;
    z = suma(x, y);
    printf("suma = %d\\n", z);
    return 0;
}`,
      compiledCode: {
        assembly: ''
      },
      isCompiling: false,
      canStep: false,
      stepCount: 0,
      
      // Simulador de ensamblador
      simulator: null,
      
      // Estado de ejecución
      executionState: {
        registers: {
          rax: 0, rbx: 0, rcx: 0, rdx: 0,
          rsi: 0, rdi: 0, rsp: 0, rbp: 0,
          rip: 0, r8: 0
        },
        currentLine: 0,
        modifiedRegisters: [],
        stack: []
      },
      
      // Notificación
      notification: {
        show: false,
        message: '',
        type: 'info'
      },
      
      // Layout
      leftWidth: 50,
      topRightHeight: 50,
      isResizing: false,
      resizeType: null
    }
  },
  
  computed: {
    compileStatus() {
      if (this.isCompiling) return 'COMPILING...';
      if (this.simulator && !this.simulator.finished) return 'READY';
      if (this.simulator && this.simulator.finished) return 'FINISHED';
      if (this.compiledCode.assembly) return 'COMPILED';
      return 'IDLE';
    }
  },
  
  methods: {
    showNotification(type, message) {
      this.notification = { show: true, type, message };
      setTimeout(() => {
        this.notification.show = false;
      }, 3000);
    },
    
    async handleCompile() {
      this.isCompiling = true;
      try {
        const result = await compilerStore.compile(this.sourceCode);
        
        console.log('Compilation result:', result);
        
        if (result.success) {
          this.compiledCode = result;
          this.canStep = true;
          this.stepCount = 0;
          
          // INICIALIZAR EL SIMULADOR CON EL CÓDIGO ENSAMBLADOR
          this.simulator = new AssemblySimulator(result.assembly);
          this.updateExecutionStateFromSimulator();
          
          this.showNotification('success', 'Compilation successful!');
        } else {
          this.canStep = false;
          this.simulator = null;
          this.showNotification('error', `Compilation error: ${result.error}`);
        }
      } catch (error) {
        console.error('Compilation error:', error);
        this.showNotification('error', `Network error: ${error.message}`);
      } finally {
        this.isCompiling = false;
      }
    },
    
    handleStepForward() {
      if (!this.simulator || this.simulator.finished) {
        console.log('Simulador no disponible o terminado');
        return;
      }
      
      // Guardar registros anteriores para detectar cambios
      const prevRegisters = { ...this.simulator.registers };
      
      // EJECUTAR UN PASO EN EL SIMULADOR
      this.simulator.step();
      this.stepCount++;
      
      // Actualizar el estado desde el simulador
      this.updateExecutionStateFromSimulator(prevRegisters);
      
      // Verificar si terminó
      if (this.simulator.finished) {
        this.canStep = false;
        this.showNotification('info', 'Ejecución terminada');
      }
    },
    
    handleStepBackward() {
      console.log('Step backward no implementado aún');
      this.showNotification('info', 'Step backward no implementado');
    },
    
    handleReset() {
      if (this.compiledCode.assembly) {
        this.simulator = new AssemblySimulator(this.compiledCode.assembly);
        this.stepCount = 0;
        this.canStep = true;
        this.updateExecutionStateFromSimulator();
        this.showNotification('success', 'Simulador reiniciado');
      }
    },
    
    updateExecutionStateFromSimulator(prevRegisters = null) {
      if (!this.simulator) return;
      
      const state = this.simulator.getCurrentState();
      
      // Detectar qué registros cambiaron
      let modifiedRegs = [];
      if (prevRegisters) {
        for (const reg in state.registers) {
          if (state.registers[reg] !== prevRegisters[reg]) {
            modifiedRegs.push(reg);
          }
        }
      }
      
      // Actualizar estado de ejecución
      this.executionState = {
        registers: { ...state.registers },
        currentLine: state.line + 1, // +1 porque las líneas en la UI empiezan en 1
        modifiedRegisters: modifiedRegs,
        stack: this.formatStackForView(state.stack)
      };
    },
    
    formatStackForView(simulatorStack) {
      if (!simulatorStack || simulatorStack.length === 0) {
        return [];
      }
      
      return simulatorStack.map(item => ({
        address: '0x' + item.address.toString(16),
        offset: item.offset,
        value: '0x' + item.value.toString(16),
        hasValue: item.value !== 0,
        isRbp: item.isRbp,
        isRsp: item.isRsp
      }));
    },
    
    startResize(type, event) {
      this.isResizing = true;
      this.resizeType = type;
      document.addEventListener('mousemove', this.onResize);
      document.addEventListener('mouseup', this.stopResize);
    },
    
    onResize(event) {
      if (!this.isResizing) return;
      
      if (this.resizeType === 'middle') {
        const containerWidth = window.innerWidth;
        const newWidth = (event.clientX / containerWidth) * 100;
        this.leftWidth = Math.max(20, Math.min(80, newWidth));
      } else if (this.resizeType === 'vertical') {
        const container = event.target.parentElement;
        if (container) {
          const containerHeight = container.offsetHeight;
          const rect = container.getBoundingClientRect();
          const newHeight = ((event.clientY - rect.top) / containerHeight) * 100;
          this.topRightHeight = Math.max(20, Math.min(80, newHeight));
        }
      }
    },
    
    stopResize() {
      this.isResizing = false;
      this.resizeType = null;
      document.removeEventListener('mousemove', this.onResize);
      document.removeEventListener('mouseup', this.stopResize);
    }
  }
}
</script>



<style>
/* Smooth scrollbar */
::-webkit-scrollbar {
  width: 8px;
  height: 8px;
}

::-webkit-scrollbar-track {
  background: #000;
}

::-webkit-scrollbar-thumb {
  background: linear-gradient(to bottom, #06b6d4, #ef4444);
  border-radius: 4px;
}

::-webkit-scrollbar-thumb:hover {
  background: linear-gradient(to bottom, #22d3ee, #f87171);
}

/* Prevent text selection during resize */
body.resizing {
  user-select: none;
  cursor: col-resize;
}

/* Animations */
@keyframes glow {
  0%, 100% {
    box-shadow: 0 0 5px rgba(6, 182, 212, 0.5);
  }
  50% {
    box-shadow: 0 0 20px rgba(6, 182, 212, 0.8);
  }
}

.animate-glow {
  animation: glow 2s ease-in-out infinite;
}
</style>