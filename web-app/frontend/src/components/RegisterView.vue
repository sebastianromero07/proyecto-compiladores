<template>
  <div class="register-view bg-gray-900 text-white p-4 rounded-lg h-full overflow-y-auto">
    <h3 class="text-lg font-bold mb-4">🔧 Registros CPU</h3>
    <div class="grid grid-cols-2 gap-3">
      <div v-for="(value, reg) in registers" :key="reg"
           class="register-item p-2 bg-gray-800 rounded"
           :class="{ 'bg-yellow-600': isModified(reg) }">
        <span class="font-mono text-sm">{{ reg }}:</span>
        <span class="font-mono ml-2 text-green-400">{{ formatValue(value) }}</span>
      </div>
    </div>
    
    <!-- Stack Pointer Visual -->
    <div class="mt-4 p-3 bg-blue-900 rounded">
      <h4 class="font-bold">Stack Pointer</h4>
      <div class="font-mono">RSP: {{ formatValue(registers.rsp) }}</div>
      <div class="font-mono">RBP: {{ formatValue(registers.rbp) }}</div>
    </div>

    <!-- Si no hay registros, mostrar mensaje -->
    <div v-if="Object.keys(registers).length === 0" class="text-center text-gray-400 mt-8">
      <p>📊 Los registros aparecerán aquí después de compilar</p>
    </div>
  </div>
</template>

<script>
export default {
  name: 'RegisterView',
  props: {
    registers: {
      type: Object,
      default: () => ({})
    },
    modifiedRegs: {
      type: Array,
      default: () => []
    }
  },
  methods: {
    formatValue(value) {
      if (typeof value === 'number') {
        return `0x${value.toString(16).padStart(8, '0')} (${value})`;
      }
      return value || '0x00000000 (0)';
    },
    isModified(reg) {
      return this.modifiedRegs?.includes(reg) || false;
    }
  }
}
</script>