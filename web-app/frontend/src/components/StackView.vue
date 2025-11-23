<template>
  <div class="stack-view bg-gray-900 text-white p-4 rounded-lg h-full overflow-y-auto">
    <h3 class="text-lg font-bold mb-4">📚 Stack Memory</h3>
    
    <div class="stack-container">
      <table class="w-full font-mono text-sm">
        <thead class="bg-gray-800">
          <tr>
            <th class="p-2 text-left">Address</th>
            <th class="p-2 text-left">Value</th>
            <th class="p-2 text-left">Variable</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="(item, index) in stackData" :key="index"
              class="border-b border-gray-700"
              :class="{ 
                'bg-yellow-800': item.isCurrentSP,
                'bg-blue-800': item.isCurrentBP,
                'bg-green-800': item.isModified 
              }">
            <td class="p-2">{{ item.address }}</td>
            <td class="p-2 text-green-400">{{ item.value }}</td>
            <td class="p-2 text-blue-400">{{ item.variable || '-' }}</td>
          </tr>
        </tbody>
      </table>
    </div>
    
    <!-- Stack Growth Indicator -->
    <div class="mt-3 text-xs text-gray-400">
      ⬇️ Stack grows downward | 
      🟡 SP | 🔵 BP | 🟢 Modified
    </div>

    <!-- Si no hay stack data -->
    <div v-if="!stackData || stackData.length === 0" class="text-center text-gray-400 mt-8">
      <p>📚 El stack aparecerá aquí durante la ejecución</p>
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
  }
}
</script>