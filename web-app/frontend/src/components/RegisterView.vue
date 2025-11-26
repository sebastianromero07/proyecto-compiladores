<template>
  <div class="bg-black h-full flex flex-col overflow-hidden">
    <div class="px-4 py-2 border-b border-red-500/30 bg-gradient-to-r from-gray-900 to-black flex-shrink-0">
      <h3 class="text-sm font-bold text-red-400 tracking-wide">REGISTERS</h3>
    </div>
    
    <div class="flex-1 overflow-y-auto p-3 custom-scrollbar bg-black">
      <div class="grid grid-cols-2 gap-2">
        <div 
          v-for="(value, name) in registers" 
          :key="name"
          :class="[
            'p-2 rounded border transition-all duration-300',
            isModified(name) 
              ? 'bg-red-500/10 border-red-500 shadow-[0_0_10px_rgba(239,68,68,0.5)]' 
              : 'bg-gray-900/50 border-gray-700 hover:border-gray-600'
          ]"
        >
          <div class="flex justify-between items-center mb-1">
            <span class="text-gray-400 font-bold uppercase text-[10px] tracking-wider">{{ name }}</span>
            <span v-if="isModified(name)" class="w-1.5 h-1.5 rounded-full bg-red-500 animate-pulse"></span>
          </div>
          <div class="font-mono text-xs">
            <div :class="isModified(name) ? 'text-red-400' : 'text-cyan-400'" class="font-semibold">
              {{ formatHex(value) }}
            </div>
            <div class="text-gray-600 text-[10px] mt-0.5">{{ value }}</div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'RegisterView',
  props: {
    registers: {
      type: Object,
      required: true
    },
    modifiedRegs: {
      type: Array,
      default: () => []
    }
  },
  methods: {
    formatHex(val) {
      if (val === undefined || val === null) return '0x0';
      return `0x${(val >>> 0).toString(16).toUpperCase().padStart(8, '0')}`;
    },
    isModified(regName) {
      return this.modifiedRegs.includes(regName);
    }
  }
}
</script>

<style scoped>
.custom-scrollbar::-webkit-scrollbar {
  width: 4px;
}
.custom-scrollbar::-webkit-scrollbar-track {
  background: #000;
}
.custom-scrollbar::-webkit-scrollbar-thumb {
  background: linear-gradient(to bottom, #ef4444, #06b6d4);
  border-radius: 2px;
}
</style>