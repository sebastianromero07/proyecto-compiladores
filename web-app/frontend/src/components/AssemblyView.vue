<template>
  <div class="assembly-view bg-black h-full flex flex-col overflow-hidden">
    <div class="px-4 py-2 border-b border-cyan-500/30 bg-gradient-to-r from-gray-900 to-black flex-shrink-0">
      <h3 class="text-sm font-bold text-cyan-400 tracking-wide">ASSEMBLY CODE</h3>
    </div>
    
    <div class="flex-1 overflow-y-auto font-mono text-xs custom-scrollbar bg-black">
      <div v-if="assemblyLines.length > 0">
        <div 
          v-for="(line, index) in assemblyLines" 
          :key="index"
          :class="[
            'flex items-start px-3 py-1 transition-all border-l-2',
            isCurrentLine(index + 1) 
              ? 'bg-cyan-500/10 border-l-cyan-500 shadow-[0_0_10px_rgba(6,182,212,0.3)]' 
              : 'border-l-transparent hover:bg-gray-900/30'
          ]"
        >
          <!-- Line Number -->
          <div class="w-12 text-gray-600 text-right mr-4 flex-shrink-0 select-none">
            {{ index + 1 }}
          </div>
          
          <!-- Assembly Code -->
          <div class="flex-1">
            <span v-if="line.isLabel" class="text-red-400 font-semibold">{{ line.text }}</span>
            <span v-else-if="line.isDirective" class="text-gray-600">{{ line.text }}</span>
            <span v-else-if="line.isEmpty" class="text-gray-800">{{ line.text }}</span>
            <span v-else class="text-gray-300">
              <span class="text-cyan-400 font-semibold">{{ line.opcode }}</span>
              <span v-if="line.operands" class="text-gray-400"> {{ line.operands }}</span>
            </span>
          </div>
        </div>
      </div>
      
      <div v-else class="flex items-center justify-center h-full text-gray-600">
        <p class="font-mono text-sm">Assembly code will appear here after compilation</p>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'AssemblyView',
  props: {
    assembly: {
      type: String,
      default: ''
    },
    currentLine: {
      type: Number,
      default: 0
    }
  },
  computed: {
    assemblyLines() {
      if (!this.assembly) return [];
      
      return this.assembly.split('\n').map(line => {
        const trimmed = line.trim();
        
        if (trimmed.endsWith(':')) {
          return {
            text: trimmed,
            isLabel: true,
            isEmpty: false,
            isDirective: false
          };
        }
        
        if (trimmed.startsWith('.')) {
          return {
            text: trimmed,
            isLabel: false,
            isEmpty: false,
            isDirective: true
          };
        }
        
        if (!trimmed) {
          return {
            text: '',
            isLabel: false,
            isEmpty: true,
            isDirective: false
          };
        }
        
        const parts = trimmed.split(/\s+/);
        const opcode = parts[0];
        const operands = parts.slice(1).join(' ');
        
        return {
          text: trimmed,
          opcode: opcode,
          operands: operands,
          isLabel: false,
          isEmpty: false,
          isDirective: false
        };
      });
    }
  },
  methods: {
    isCurrentLine(lineNum) {
      return this.currentLine === lineNum;
    }
  }
}
</script>

<style scoped>
.custom-scrollbar::-webkit-scrollbar {
  width: 6px;
}
.custom-scrollbar::-webkit-scrollbar-track {
  background: #000;
}
.custom-scrollbar::-webkit-scrollbar-thumb {
  background: linear-gradient(to bottom, #06b6d4, #ef4444);
  border-radius: 3px;
}
</style>