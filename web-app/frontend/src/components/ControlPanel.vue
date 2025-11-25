<template>
  <div class="control-panel bg-slate-800 p-4 border-b border-slate-700">
    <div class="flex gap-3 items-center">
      <button 
        @click="$emit('step-backward')"
        :disabled="!canStep"
        class="bg-orange-600 hover:bg-orange-700 disabled:bg-gray-600 px-4 py-2 rounded transition-colors flex items-center gap-2"
      >
        ⬅️ Step Back
      </button>
      
      <button 
        @click="$emit('step-forward')"
        :disabled="!canStep"
        class="bg-green-600 hover:bg-green-700 disabled:bg-gray-600 px-4 py-2 rounded transition-colors flex items-center gap-2"
      >
        ➡️ Step Forward
      </button>
      
      <button 
        @click="$emit('reset')"
        class="bg-red-600 hover:bg-red-700 px-4 py-2 rounded transition-colors flex items-center gap-2"
      >
        🔄 Reset
      </button>

      <div class="ml-auto text-sm text-gray-400">
        {{ canStep ? '✅ Listo para debuggear' : '⏳ Compila primero' }}
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'ControlPanel',
  props: {
    canStep: {
      type: Boolean,
      default: false
    }
  },
  emits: ['step-forward', 'step-backward', 'reset']
}
</script>