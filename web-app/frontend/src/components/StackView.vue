<template>
  <div class="bg-black h-full flex flex-col overflow-hidden">
    <div class="px-4 py-2 border-b border-red-500/30 bg-gradient-to-r from-gray-900 to-black flex-shrink-0">
      <h3 class="text-sm font-bold text-red-400 tracking-wide">STACK MEMORY</h3>
    </div>
    
    <div class="flex-1 overflow-y-auto font-mono text-[10px] p-3 custom-scrollbar bg-black">
      <div v-if="stackData.length === 0" class="text-gray-600 text-center py-8 text-xs">
        Stack empty
      </div>
      
      <div v-else class="space-y-0.5">
        <div 
          v-for="(item, index) in stackData" 
          :key="index"
          :class="[
            'flex items-center justify-between px-2 py-1.5 rounded border transition-all',
            item.isRsp ? 'bg-cyan-500/10 border-cyan-500 shadow-[0_0_8px_rgba(6,182,212,0.4)]' : 
            item.isRbp ? 'bg-red-500/10 border-red-500 shadow-[0_0_8px_rgba(239,68,68,0.4)]' : 
            item.hasValue ? 'bg-gray-900/50 border-gray-700' : 'border-gray-800'
          ]"
        >
          <!-- Address + Offset -->
          <div class="flex items-center gap-3">
            <span class="text-gray-500 w-12">0x{{ item.address }}</span>
            <span :class="item.offset === 0 ? 'text-red-400' : item.offset > 0 ? 'text-cyan-400' : 'text-gray-400'" class="w-10 text-right font-semibold">
              {{ formatOffset(item.offset) }}
            </span>
          </div>
          
          <!-- Markers -->
          <div class="flex gap-1.5">
            <span v-if="item.isRsp" class="text-[9px] bg-cyan-500 px-1.5 py-0.5 rounded text-black font-bold">RSP</span>
            <span v-if="item.isRbp" class="text-[9px] bg-red-500 px-1.5 py-0.5 rounded text-black font-bold">RBP</span>
          </div>
          
          <!-- Value -->
          <div class="text-right">
            <span v-if="item.hasValue" :class="item.isRsp ? 'text-cyan-400' : item.isRbp ? 'text-red-400' : 'text-gray-300'" class="font-semibold">
              0x{{ item.value }}
            </span>
            <span v-else class="text-gray-700">---</span>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'StackView',
  props: {
    stackData: {
      type: Array,
      default: () => []
    }
  },
  methods: {
    formatOffset(offset) {
      if (offset === 0) return '0';
      if (offset > 0) return `+${offset}`;
      return `${offset}`;
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