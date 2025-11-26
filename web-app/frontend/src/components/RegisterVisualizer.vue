<template>
  <div class="bg-slate-800 rounded-lg p-4 h-full overflow-y-auto custom-scrollbar">
    <h3 class="text-sm font-semibold text-slate-400 mb-3 flex items-center gap-2">
      <span>🔧</span> Registers
    </h3>
    
    <div class="grid grid-cols-2 gap-3">
      <div 
        v-for="(value, name) in registers" 
        :key="name"
        :class="[
          'p-2 rounded border transition-all duration-300',
          isModified(name) ? 'bg-yellow-900/20 border-yellow-500/50' : 'bg-slate-700/30 border-slate-700'
        ]"
      >
        <div class="flex justify-between items-center mb-1">
          <span class="text-slate-400 font-bold uppercase text-xs">{{ name }}</span>
          <span v-if="isModified(name)" class="w-2 h-2 rounded-full bg-yellow-500"></span>
        </div>
        <div class="font-mono text-sm text-right truncate">
          <span class="text-emerald-400">{{ formatHex(value) }}</span>
        </div>
        <div class="text-xs text-slate-500 text-right font-mono">
          {{ value }}
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'RegisterVisualizer',
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
      return `0x${(val >>> 0).toString(16).toUpperCase()}`;
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
  background: #1e293b;
}
.custom-scrollbar::-webkit-scrollbar-thumb {
  background: #475569;
  border-radius: 2px;
}
</style>
