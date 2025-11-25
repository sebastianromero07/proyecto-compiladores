<template>
  <div class="min-h-screen bg-slate-900 text-white">
    <!-- Header -->
    <header class="bg-slate-800 border-b border-slate-700 p-4">
      <div class="flex justify-between items-center">
        <h1 class="text-2xl font-bold">🔧 Compilador de C - Visualizer</h1>
        <div class="text-sm text-gray-400">
          {{ compileStatus }}
        </div>
      </div>
    </header>

    <!-- Notification Toast -->
    <div v-if="notification.show" 
         :class="[
           'fixed top-20 right-4 p-4 rounded-lg shadow-lg z-50 transition-all duration-300',
           notification.type === 'success' ? 'bg-green-600' : 'bg-red-600'
         ]">
      <div class="flex items-center gap-2">
        <span>{{ notification.type === 'success' ? '✅' : '❌' }}</span>
        <span>{{ notification.message }}</span>
        <button @click="hideNotification" class="ml-2 text-white hover:text-gray-200">✕</button>
      </div>
    </div>

    <!-- Main Layout -->
    <div class="flex" style="height: calc(100vh - 80px);">
      <!-- Left Panel - Code Editor -->
      <div class="w-1/2 border-r border-slate-700">
        <CodeEditor 
          v-model="sourceCode" 
          @compile="handleCompile"
          @download="handleDownload"
          :isLoading="isCompiling"
        />
      </div>

      <!-- Right Panel - Visualization -->
      <div class="w-1/2 flex flex-col">
        <!-- Control Panel -->
        <ControlPanel 
          @step-forward="stepForward"
          @step-backward="stepBackward" 
          @reset="reset"
          :canStep="canStep"
          :executionInfo="executionInfo"
        />

        <!-- Tabs -->
        <div class="flex border-b border-slate-700">
          <button 
            v-for="tab in tabs" 
            :key="tab.id"
            @click="activeTab = tab.id"
            :class="[
              'px-4 py-2 border-b-2 transition-colors flex items-center gap-2',
              activeTab === tab.id 
                ? 'border-blue-500 text-blue-400' 
                : 'border-transparent hover:text-blue-300'
            ]"
          >
            <span>{{ tab.icon }}</span>
            <span>{{ tab.name }}</span>
            <span v-if="tab.badge" class="bg-blue-600 text-xs px-1 rounded">
              {{ tab.badge }}
            </span>
          </button>
        </div>

        <!-- Tab Content -->
        <div class="flex-1 overflow-hidden">
          <RegisterView 
            v-if="activeTab === 'registers'"
            :registers="executionState.registers"
            :modifiedRegs="modifiedRegs"
          />
          
          <StackView 
            v-if="activeTab === 'stack'"
            :stackData="executionState.stack"
          />
          
          <AssemblyView 
            v-if="activeTab === 'assembly'"
            :assembly="compiledCode.assembly"
            :currentLine="executionState.currentLine"
          />
          
          <TokenView 
            v-if="activeTab === 'tokens'"
            :tokens="compiledCode.tokens"
          />
        </div>
      </div>
    </div>

    <!-- Loading Overlay -->
    <div v-if="isCompiling" class="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50">
      <div class="bg-slate-800 p-6 rounded-lg flex items-center gap-3">
        <div class="animate-spin rounded-full h-6 w-6 border-b-2 border-blue-500"></div>
        <span>Compilando código...</span>
      </div>
    </div>
  </div>
</template>

<script>
import CodeEditor from './components/CodeEditor.vue'
import RegisterView from './components/RegisterView.vue'
import StackView from './components/StackView.vue'
import AssemblyView from './components/AssemblyView.vue'
import TokenView from './components/TokenView.vue'
import ControlPanel from './components/ControlPanel.vue'
import { compilerStore } from './stores/compiler.js'

export default {
  name: 'App',
  components: {
    CodeEditor,
    RegisterView, 
    StackView,
    AssemblyView,
    TokenView,
    ControlPanel
  },
  data() {
    return {
      sourceCode: `#include<stdio.h>

int main(){
    int x = 5;
    int y = 10;
    
    if (x > y) {
        printf("x es mayor\\n");
    } else {
        printf("y es mayor: %d\\n", y);
    }
    
    return 0;
}`,
      activeTab: 'tokens', // Empezar con tokens para ver resultados inmediatamente
      isCompiling: false,
      
      // Notificaciones
      notification: {
        show: false,
        type: 'success', // 'success' | 'error'
        message: ''
      },
      
      // Estado de ejecución
      executionState: {
        registers: {
          rax: 0, rbx: 0, rcx: 0, rdx: 0,
          rsi: 0, rdi: 0, rsp: 1000, rbp: 1000
        },
        stack: [],
        currentLine: 0,
        variables: {}
      },
      
      // Código compilado
      compiledCode: {
        assembly: '',
        tokens: [],
        executionSteps: []
      },
      
      modifiedRegs: [],
      canStep: false,
      stepCount: 0
    }
  },
  
  computed: {
    // Tabs con badges dinámicos
    tabs() {
      return [
        { 
          id: 'tokens', 
          name: 'Tokens',
          icon: '🏷️',
          badge: this.compiledCode.tokens?.length || null
        },
        { 
          id: 'assembly', 
          name: 'Assembly',
          icon: '⚙️',
          badge: this.compiledCode.assembly ? 'OK' : null
        },
        { 
          id: 'registers', 
          name: 'Registros',
          icon: '🔧',
          badge: this.canStep ? 'LIVE' : null
        },
        { 
          id: 'stack', 
          name: 'Stack',
          icon: '📚',
          badge: this.executionState.stack?.length || null
        }
      ];
    },
    
    // Status de compilación
    compileStatus() {
      if (this.isCompiling) return '🔨 Compilando...';
      if (this.canStep) return '✅ Listo para debug';
      if (this.compiledCode.tokens?.length > 0) return '📝 Compilado';
      return '⏳ Esperando código';
    },
    
    // Info de ejecución
    executionInfo() {
      return {
        stepCount: this.stepCount,
        currentLine: this.executionState.currentLine,
        canStepBack: this.stepCount > 0,
        hasExecution: this.canStep
      };
    }
  },
  
  methods: {
    // Sistema de notificaciones
    $notify(type, message) {
      this.notification = {
        show: true,
        type: type,
        message: message
      };
      
      // Auto-hide después de 4 segundos
      setTimeout(() => {
        this.hideNotification();
      }, 4000);
      
      // También log en consola
      console.log(`[${type.toUpperCase()}] ${message}`);
    },
    
    hideNotification() {
      this.notification.show = false;
    },

    // Compilación
    async handleCompile() {
      this.isCompiling = true;
      this.hideNotification();
      
      try {
        console.log('🔨 Iniciando compilación...');
        const result = await compilerStore.compile(this.sourceCode);
        
        console.log('📊 Resultado de compilación:', result);
        
        if (result.success) {
          this.compiledCode = result;
          this.canStep = true;
          this.stepCount = 0;
          
          // Cambiar a tab de tokens para mostrar resultados
          this.activeTab = 'tokens';
          
          // Inicializar estado de ejecución simulado
          this.initializeExecutionState();
          
          this.$notify('success', `✅ Compilación exitosa! ${result.tokens?.length || 0} tokens generados`);
        } else {
          this.canStep = false;
          this.$notify('error', `❌ Error de compilación: ${result.error}`);
        }
      } catch (error) {
        console.error('Error en compilación:', error);
        this.$notify('error', `❌ Error de red: ${error.message}`);
      } finally {
        this.isCompiling = false;
      }
    },

    // Inicializar estado de ejecución
    initializeExecutionState() {
      this.executionState = {
        registers: {
          rax: 0, rbx: 0, rcx: 0, rdx: 0,
          rsi: 0, rdi: 0, rsp: 1000, rbp: 1000
        },
        stack: [
          { address: '0x1000', value: '0x00000000', variable: 'main_frame' }
        ],
        currentLine: 1,
        variables: {}
      };
      this.modifiedRegs = [];
    },

    // Debugging paso a paso
    async stepForward() {
      if (!this.canStep) {
        this.$notify('error', '❌ Compila el código primero');
        return;
      }

      try {
        const result = await compilerStore.stepForward();
        if (result.success) {
          this.executionState = result.state;
          this.modifiedRegs = result.modifiedRegs || ['rax', 'rcx'];
          this.stepCount++;
          
          // Simular progreso
          this.executionState.currentLine = this.stepCount;
          this.executionState.registers.rax = this.stepCount * 10;
          
          this.$notify('success', `➡️ Step ${this.stepCount}`);
        }
      } catch (error) {
        this.$notify('error', '❌ Error en step forward');
      }
    },

    async stepBackward() {
      if (this.stepCount <= 0) {
        this.$notify('error', '❌ No hay pasos anteriores');
        return;
      }

      try {
        const result = await compilerStore.stepBackward();
        if (result.success) {
          this.stepCount--;
          this.executionState.currentLine = Math.max(1, this.stepCount);
          this.executionState.registers.rax = Math.max(0, this.stepCount * 10);
          
          this.$notify('success', `⬅️ Back to step ${this.stepCount}`);
        }
      } catch (error) {
        this.$notify('error', '❌ Error en step backward');
      }
    },

    // Reset
    reset() {
      this.stepCount = 0;
      this.initializeExecutionState();
      this.canStep = false;
      this.modifiedRegs = [];
      this.compiledCode = {
        assembly: '',
        tokens: [],
        executionSteps: []
      };
      
      this.activeTab = 'tokens';
      this.$notify('success', '🔄 Estado reiniciado');
    },
    
    // Descargar resultados
    handleDownload() {
      if (!this.compiledCode.tokens || this.compiledCode.tokens.length === 0) {
        this.$notify('error', '❌ No hay resultados para descargar. Compila primero.');
        return;
      }
      
      try {
        const data = {
          sourceCode: this.sourceCode,
          tokens: this.compiledCode.tokens,
          assembly: this.compiledCode.assembly,
          timestamp: new Date().toISOString(),
          stepCount: this.stepCount
        };
        
        const blob = new Blob([JSON.stringify(data, null, 2)], {
          type: 'application/json'
        });
        
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = `compiler_results_${Date.now()}.json`;
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(url);
        
        this.$notify('success', '📥 Resultados descargados');
      } catch (error) {
        this.$notify('error', '❌ Error descargando archivo');
      }
    }
  },
  
  // Al montar el componente
  mounted() {
    console.log('🚀 App mounted. Compilador visualizer listo.');
    
    // Cargar código desde URL si existe
    const urlParams = new URLSearchParams(window.location.search);
    const encodedCode = urlParams.get('code');
    
    if (encodedCode) {
      try {
        const decodedCode = decodeURIComponent(atob(encodedCode));
        this.sourceCode = decodedCode;
        this.$notify('success', '🔗 Código cargado desde URL');
      } catch (e) {
        console.warn('No se pudo decodificar código de URL');
      }
    }
  }
}
</script>

<style>
/* Animaciones para las notificaciones */
.notification-enter-active, .notification-leave-active {
  transition: all 0.3s ease;
}

.notification-enter-from {
  opacity: 0;
  transform: translateX(100%);
}

.notification-leave-to {
  opacity: 0;
  transform: translateX(100%);
}

/* Scroll suave */
.overflow-y-auto {
  scrollbar-width: thin;
  scrollbar-color: #475569 #1e293b;
}

.overflow-y-auto::-webkit-scrollbar {
  width: 6px;
}

.overflow-y-auto::-webkit-scrollbar-track {
  background: #1e293b;
}

.overflow-y-auto::-webkit-scrollbar-thumb {
  background: #475569;
  border-radius: 3px;
}
</style>