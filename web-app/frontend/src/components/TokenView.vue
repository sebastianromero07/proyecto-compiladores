<template>
  <div class="token-view bg-gray-900 text-white p-4 rounded-lg h-full overflow-y-auto">
    <h3 class="text-lg font-bold mb-4">🏷️ Tokens</h3>
    
    <div class="tokens-list">
      <div v-for="(token, index) in tokens" :key="index"
           class="token-item p-2 mb-1 bg-gray-800 rounded font-mono text-sm">
        {{ token }}
      </div>
      
      <div v-if="!tokens || tokens.length === 0" class="text-center text-gray-400 mt-8">
        <p>🏷️ Los tokens aparecerán aquí después de compilar</p>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'TokenView',
  props: {
    tokens: {
      type: Array,
      default: () => []
    }
  }
}
</script>