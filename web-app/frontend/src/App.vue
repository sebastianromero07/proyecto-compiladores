<template>
  <div class="min-h-screen bg-black text-gray-100 flex flex-col">
    <!-- Header -->
    <header class="bg-gradient-to-r from-gray-900 via-black to-gray-900 border-b border-cyan-500/30 px-6 py-3 flex-shrink-0">
      <div class="flex justify-between items-center">
        <h1 class="text-xl font-bold text-transparent bg-clip-text bg-gradient-to-r from-cyan-400 to-red-500">
          C to Assembly Compiler Visualization
        </h1>
        <div class="text-sm text-cyan-400/70 font-mono">
          {{ compileStatus }}
        </div>
      </div>
    </header>

    <!-- Notification Toast -->
    <div v-if="notification.show" 
         :class="[
           'fixed top-20 right-4 px-6 py-3 rounded-lg shadow-2xl z-50 transition-all duration-300 border',
           notification.type === 'success' 
             ? 'bg-black/90 border-cyan-500 text-cyan-400' 
             : 'bg-black/90 border-red-500 text-red-400'
         ]">
      <div class="flex items-center gap-3">
        <span class="font-mono">{{ notification.message }}</span>
        <button @click="hideNotification" class="ml-2 hover:text-white transition-colors">×</button>
      </div>
    </div>

    <!-- Main Layout with Resizable Panels -->
    <div class="flex flex-1 overflow-hidden">
      <!-- Left Panel: Code Editor -->
      <div :style="{ width: leftWidth + '%' }" class="flex-shrink-0 overflow-hidden bg-black">
        <CodeEditor 
          v-model="sourceCode" 
          @compile="handleCompile"
          @download="handleDownload"
          :isLoading="isCompiling"
        />
      </div>

      <!-- Resize Handle 1 -->
      <div 
        class="w-1 bg-cyan-500/20 hover:bg-cyan-500 cursor-col-resize transition-colors flex-shrink-0"
        @mousedown="startResize('left', $event)"
      ></div>

      <!-- Middle Panel: Assembly + Controls -->
      <div :style="{ width: middleWidth + '%' }" class="flex-shrink-0 flex flex-col overflow-hidden bg-black">
        <!-- Control Panel -->
        <div class="border-b border-cyan-500/20 bg-gray-900/50 flex-shrink-0">
          <ControlPanel 
            @step-forward="stepForward"
            @step-backward="stepBackward" 
            @reset="reset"
            @compile="handleCompile"
            :canStep="canStep"
            :executionInfo="executionInfo"
            :isCompiling="isCompiling"
          />
        </div>

        <!-- Assembly View -->
        <div class="flex-1 overflow-hidden">
          <AssemblyView 
            :assembly="compiledCode.assembly"
            :currentLine="executionState.currentLine"
          />
        </div>
      </div>

      <!-- Resize Handle 2 -->
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
          class="h-1 bg-red-500/20 hover:bg-red-500 cursor-row-resize transition-colors flex-shrink-0"
          @mousedown="startResize('vertical', $event)"
        ></div>

        <!-- Stack -->
        <div class="flex-1 overflow-hidden">
          <StackView 
            :stackData="executionState.stack"
          />
        </div>
      </div>
    </div>

    <!-- Loading Overlay -->
    <div v-if="isCompiling" class="fixed inset-0 bg-black/80 backdrop-blur-sm flex items-center justify-center z-50">
      <div class="bg-gray-900 border border-cyan-500 p-6 rounded-lg flex items-center gap-4">
        <div class="w-8 h-8 border-4 border-cyan-500 border-t-transparent rounded-full animate-spin"></div>
        <span class="text-cyan-400 font-mono">Compiling code...</span>
      </div>
    </div>
  </div>
</template>

<script>
import CodeEditor from './components/CodeEditor.vue'
import RegisterView from './components/RegisterView.vue'
import StackView from './components/StackView.vue'
import AssemblyView from './components/AssemblyView.vue'
import ControlPanel from './components/ControlPanel.vue'
import { compilerStore } from './stores/compiler.js'

export default {
  name: 'App',
  components: {
    CodeEditor,
    RegisterView, 
    StackView,
    AssemblyView,
    ControlPanel
  },
  data() {
    return {
      sourceCode: `#include<stdio.h>

int sum(int a, int b) {
    return a + b;
}

int main(){
    int x = 5;
    int y = 10;
    int result = sum(x, y);
    printf("Result: %d\\n", result);
    return 0;
}`,
      isCompiling: false,
      
      notification: {
        show: false,
        type: 'success',
        message: ''
      },
      
      executionState: {
        registers: {
          rax: 0, rbx: 0, rcx: 0, rdx: 0,
          rsi: 0, rdi: 0, rsp: 1000, rbp: 1000
        },
        stack: [],
        currentLine: 0,
        modifiedRegisters: []
      },
      
      compiledCode: {
        assembly: '',
        tokens: []
      },
      
      canStep: false,
      stepCount: 0,

      // Resizable panel widths/heights
      leftWidth: 30,
      middleWidth: 40,
      topRightHeight: 50,
      
      // Resize state
      isResizing: false,
      resizeType: null,
      startX: 0,
      startY: 0,
      startLeftWidth: 0,
      startMiddleWidth: 0,
      startTopRightHeight: 0
    }
  },
  
  computed: {
    compileStatus() {
      if (this.isCompiling) return 'COMPILING...';
      if (this.canStep) return 'READY';
      if (this.compiledCode.tokens?.length > 0) return 'COMPILED';
      return 'IDLE';
    },
    
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
    $notify(type, message) {
      this.notification = {
        show: true,
        type: type,
        message: message
      };
      
      setTimeout(() => {
        this.hideNotification();
      }, 4000);
      
      console.log(`[${type.toUpperCase()}] ${message}`);
    },
    
    hideNotification() {
      this.notification.show = false;
    },

    // Resize methods
    startResize(type, event) {
      this.isResizing = true;
      this.resizeType = type;
      this.startX = event.clientX;
      this.startY = event.clientY;
      this.startLeftWidth = this.leftWidth;
      this.startMiddleWidth = this.middleWidth;
      this.startTopRightHeight = this.topRightHeight;
      
      document.addEventListener('mousemove', this.handleResize);
      document.addEventListener('mouseup', this.stopResize);
      event.preventDefault();
    },

    handleResize(event) {
      if (!this.isResizing) return;

      if (this.resizeType === 'left') {
        const containerWidth = window.innerWidth;
        const deltaX = event.clientX - this.startX;
        const deltaPercent = (deltaX / containerWidth) * 100;
        
        const newLeftWidth = Math.max(15, Math.min(50, this.startLeftWidth + deltaPercent));
        const diff = newLeftWidth - this.startLeftWidth;
        
        this.leftWidth = newLeftWidth;
        this.middleWidth = Math.max(20, this.startMiddleWidth - diff);
      } else if (this.resizeType === 'middle') {
        const containerWidth = window.innerWidth;
        const deltaX = event.clientX - this.startX;
        const deltaPercent = (deltaX / containerWidth) * 100;
        
        this.middleWidth = Math.max(20, Math.min(60, this.startMiddleWidth + deltaPercent));
      } else if (this.resizeType === 'vertical') {
        const containerHeight = window.innerHeight - 60; // Subtract header height
        const deltaY = event.clientY - this.startY;
        const deltaPercent = (deltaY / containerHeight) * 100;
        
        this.topRightHeight = Math.max(20, Math.min(80, this.startTopRightHeight + deltaPercent));
      }
    },

    stopResize() {
      this.isResizing = false;
      this.resizeType = null;
      document.removeEventListener('mousemove', this.handleResize);
      document.removeEventListener('mouseup', this.stopResize);
    },

    async handleCompile() {
      this.isCompiling = true;
      this.hideNotification();
      
      try {
        console.log('Initiating compilation...');
        const result = await compilerStore.compile(this.sourceCode);
        
        console.log('Compilation result:', result);
        
        if (result.success) {
          this.compiledCode = result;
          this.canStep = true;
          this.stepCount = 0;
          
          this.initializeExecutionState();
          
          await compilerStore.startExecution(result.assembly);
          
          this.$notify('success', `Compilation successful! ${result.tokens?.length || 0} tokens generated`);
        } else {
          this.canStep = false;
          this.$notify('error', `Compilation error: ${result.error}`);
        }
      } catch (error) {
        console.error('Compilation error:', error);
        this.$notify('error', `Network error: ${error.message}`);
      } finally {
        this.isCompiling = false;
      }
    },

    initializeExecutionState() {
      this.executionState = {
        registers: {
          rax: 0, rbx: 0, rcx: 0, rdx: 0,
          rsi: 0, rdi: 0, rsp: 1000, rbp: 1000
        },
        stack: [],
        currentLine: 0,
        modifiedRegisters: []
      };
    },

    async stepForward() {
      if (!this.canStep) {
        this.$notify('error', 'Compile the code first');
        return;
      }

      try {
        const result = await compilerStore.stepForward();
        if (result.success) {
          this.executionState = result.state;
          this.stepCount++;
          
          this.$notify('success', `Step ${this.stepCount}`);
        } else {
          this.$notify('error', result.error || 'Error in step forward');
        }
      } catch (error) {
        this.$notify('error', 'Error in step forward');
      }
    },

    async stepBackward() {
      if (this.stepCount <= 0) {
        this.$notify('error', 'No previous steps');
        return;
      }

      try {
        const result = await compilerStore.stepBackward();
        if (result.success) {
          this.stepCount--;
          this.executionState = result.state;
          
          this.$notify('success', `Back to step ${this.stepCount}`);
        } else {
          this.$notify('error', result.error || 'Error in step backward');
        }
      } catch (error) {
        this.$notify('error', 'Error in step backward');
      }
    },

    reset() {
      this.stepCount = 0;
      this.initializeExecutionState();
      this.canStep = false;
      this.compiledCode = {
        assembly: '',
        tokens: []
      };
      
      this.$notify('success', 'State reset');
    },
    
    handleDownload() {
      if (!this.compiledCode.tokens || this.compiledCode.tokens.length === 0) {
        this.$notify('error', 'No results to download. Compile first.');
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
        
        this.$notify('success', 'Results downloaded');
      } catch (error) {
        this.$notify('error', 'Error downloading file');
      }
    }
  },
  
  mounted() {
    console.log('App mounted. Compiler visualizer ready.');
    
    const urlParams = new URLSearchParams(window.location.search);
    const encodedCode = urlParams.get('code');
    
    if (encodedCode) {
      try {
        const decodedCode = decodeURIComponent(atob(encodedCode));
        this.sourceCode = decodedCode;
        this.$notify('success', 'Code loaded from URL');
      } catch (e) {
        console.warn('Could not decode code from URL');
      }
    }
  },

  beforeUnmount() {
    document.removeEventListener('mousemove', this.handleResize);
    document.removeEventListener('mouseup', this.stopResize);
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