<template>
  <div class="bg-slate-800 rounded-lg p-4 h-full flex flex-col">
    <h3 class="text-sm font-semibold text-slate-400 mb-3 flex items-center gap-2">
      <span>📚</span> Stack Memory
    </h3>
    
    <div class="flex-1 overflow-y-auto font-mono text-sm space-y-1 pr-2 custom-scrollbar">
      <div v-if="stackData.length === 0" class="text-slate-500 text-center py-8 italic">
        Stack empty
      </div>
      
      <div 
        v-for="item in stackData" 
        :key="item.address"
        :class="[
          'flex items-center justify-between p-2 rounded border transition-all duration-300',
          item.isRsp ? 'bg-blue-900/30 border-blue-500/50' : 'bg-slate-700/30 border-slate-700',
          item.isRbp ? 'border-l-4 border-l-purple-500' : ''
        ]"
      >
        <!-- Address -->
        <div class="flex flex-col">
          <span class="text-slate-400 text-xs">{{ formatAddress(item.address) }}</span>
          <div class="flex gap-2 mt-1">
            <span v-if="item.isRsp" class="text-[10px] bg-blue-600 px-1 rounded text-white font-bold">RSP</span>
            <span v-if="item.isRbp" class="text-[10px] bg-purple-600 px-1 rounded text-white font-bold">RBP</span>
          </div>
        </div>
        
        <!-- Value -->
        <div class="text-right">
          <span class="text-emerald-400 font-bold">{{ formatValue(item.value) }}</span>
          <div class="text-xs text-slate-500">{{ item.value }}</div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'StackVisualizer',
  props: {
    stackData: {
      type: Array,
      default: () => []
    }
  },
  methods: {
    formatAddress(addr) {
      return `0x${addr.toString(16).toUpperCase()}`;
    },
    formatValue(val) {
      // Try to show as hex if it looks like a pointer or large number
      if (Math.abs(val) > 1000) {
          return `0x${(val >>> 0).toString(16).toUpperCase()}`;
      }
      return val;
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
