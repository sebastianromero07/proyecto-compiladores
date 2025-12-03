<template>
  <div class="h-full flex flex-col bg-black overflow-hidden">
    <!-- Header con botón de Guardar -->
    <div class="bg-gradient-to-r from-gray-900 to-black px-4 py-2 border-b border-cyan-500/30 flex-shrink-0 flex justify-between items-center">
      <h3 class="text-sm font-bold text-cyan-400 tracking-wide">SOURCE CODE</h3>
      <div v-if="currentFile" class="flex items-center gap-2">
        <span class="text-xs text-gray-500">{{ currentFile }}</span>
        <button 
          @click="saveCurrentFile" 
          class="text-xs bg-cyan-900/50 hover:bg-cyan-700 text-cyan-200 px-2 py-1 rounded border border-cyan-500/30 transition-colors"
        >
          💾 Save
        </button>
      </div>
    </div>
    
    
    <!-- ...existing code (Area de texto)... -->
    <div class="flex-1 overflow-hidden bg-black">
      <div class="h-full flex overflow-hidden bg-black">
        <!-- Line numbers -->
        <div class="bg-gray-900/50 px-3 py-4 border-r border-cyan-500/20 overflow-y-auto flex-shrink-0" style="scrollbar-width: none;">
          <div class="font-mono text-xs text-gray-600 space-y-0.5">
            <div v-for="n in lineCount" :key="n" class="leading-6">{{ n }}</div>
          </div>
        </div>
        
        <!-- Code area -->
        <textarea 
          v-model="localCode"
          @input="handleInput"
          class="flex-1 overflow-y-auto bg-black text-gray-100 font-mono text-sm p-4 resize-none outline-none border-none leading-6 overflow-y-auto"
          placeholder="Select a file or write code..."
          spellcheck="false"
        ></textarea>
      </div>
    </div>

    <!-- Footer: Lista de Inputs Dinámica -->
    <div class="bg-gradient-to-r from-gray-900 to-black px-4 py-2 border-t border-cyan-500/30 flex-shrink-0">
      <div class="flex gap-2 text-xs items-center overflow-x-auto pb-1" style="scrollbar-width: thin;">
        <span class="text-gray-500 mr-2 font-mono flex-shrink-0">INPUTS:</span>
        
        <button 
          v-for="file in inputFiles" 
          :key="file"
          @click="loadFile(file)"
          :class="[
            'px-3 py-1 rounded transition-all font-mono border whitespace-nowrap',
            currentFile === file 
              ? 'bg-cyan-900/80 border-cyan-400 text-white' 
              : 'bg-gray-800 hover:bg-gray-700 border-cyan-500/30 hover:border-cyan-500 text-cyan-400'
          ]"
        >
          {{ file }}
        </button>
      </div>
    </div>
  </div>
</template>

<script>
import { compilerStore } from '../stores/compiler';

export default {
  props: {
    modelValue: String,
    isLoading: Boolean
  },
  emits: ['update:modelValue', 'compile', 'download'],
  data() {
    return {
      localCode: this.modelValue,
      inputFiles: [],
      currentFile: null
    }
  },
  computed: {
    lineCount() {
      return (this.localCode || '').split('\n').length;
    }
  },
  watch: {
    modelValue(newVal) {
      this.localCode = newVal;
    }
  },
  async mounted() {
    await this.refreshInputs();
  },
  methods: {
    handleInput() {
      this.$emit('update:modelValue', this.localCode);
    },
    async refreshInputs() {
      this.inputFiles = await compilerStore.listInputs();
    },
    async loadFile(filename) {
      try {
        const content = await compilerStore.loadInput(filename);
        this.localCode = content;
        this.currentFile = filename;
        this.$emit('update:modelValue', this.localCode);
      } catch (e) {
        alert('Error cargando archivo');
      }
    },
    async saveCurrentFile() {
      if (!this.currentFile) return;
      try {
        await compilerStore.saveInput(this.currentFile, this.localCode);
        // Opcional: Mostrar notificación de éxito
        console.log('Archivo guardado');
      } catch (e) {
        alert('Error guardando archivo');
      }
    }
  }
}
</script>

<style scoped>
textarea {
  tab-size: 4;
  caret-color: #06b6d4;
}
/* ...existing styles... */
</style>