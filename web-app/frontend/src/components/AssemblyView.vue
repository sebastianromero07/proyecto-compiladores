<template>
  <div class="assembly-view bg-gray-900 text-white p-4 rounded-lg h-full overflow-y-auto">
    <h3 class="text-lg font-bold mb-4">⚙️ Código Assembly</h3>
    
    <div class="assembly-code font-mono text-sm">
      <pre v-if="assembly" class="whitespace-pre-wrap">{{ assembly }}</pre>
      
      <div v-else class="text-center text-gray-400 mt-8">
        <p>⚙️ El código assembly aparecerá aquí después de compilar</p>
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
  }
}
</script>