<template>
  <div class="control-panel h-full flex items-center px-4 py-3 gap-4">
    <!-- Step Controls -->
    <div class="flex gap-2">
      <button 
        @click="$emit('step-backward')"
        :disabled="!executionInfo.canStepBack"
        class="px-4 py-2 bg-gray-900 border border-cyan-500/30 hover:border-cyan-500 disabled:border-gray-700 disabled:text-gray-600 disabled:cursor-not-allowed rounded transition-all text-cyan-400 font-mono text-sm font-semibold"
      >
        ← BACK
      </button>
      
      <button 
        @click="$emit('step-forward')"
        :disabled="!canStep"
        class="px-4 py-2 bg-gray-900 border border-red-500/30 hover:border-red-500 disabled:border-gray-700 disabled:text-gray-600 disabled:cursor-not-allowed rounded transition-all text-red-400 font-mono text-sm font-semibold"
      >
        FORWARD →
      </button>

      <button 
        @click="$emit('compile')"
        :disabled="isCompiling"
        class="px-6 py-2 bg-gradient-to-r from-cyan-600 to-cyan-500 hover:from-cyan-500 hover:to-cyan-400 disabled:from-gray-700 disabled:to-gray-600 disabled:cursor-not-allowed rounded transition-all text-black font-mono text-sm font-bold shadow-lg shadow-cyan-500/50"
      >
        {{ isCompiling ? 'COMPILING...' : 'COMPILE' }}
      </button>
    </div>

    <!-- Execution Info -->
    <div class="ml-auto flex items-center gap-6 text-xs font-mono">
      <div class="flex items-center gap-2">
        <span class="text-gray-500">STEP:</span>
        <span class="text-cyan-400 font-bold text-sm">{{ executionInfo.stepCount }}</span>
      </div>
      <div class="flex items-center gap-2">
        <span class="text-gray-500">LINE:</span>
        <span class="text-red-400 font-bold text-sm">{{ executionInfo.currentLine }}</span>
      </div>
      <div class="flex items-center gap-2">
        <span class="text-gray-500">STATUS:</span>
        <span :class="canStep ? 'text-green-400' : 'text-gray-500'" class="font-bold text-sm">
          {{ canStep ? 'READY' : 'IDLE' }}
        </span>
      </div>
    </div>

    <!-- Reset Button -->
    <button 
      @click="$emit('reset')"
      class="px-4 py-2 bg-gray-900 border border-red-500/30 hover:border-red-500 rounded transition-all text-red-400 font-mono text-sm"
    >
      RESET
    </button>
  </div>
</template>

<script>
export default {
  name: 'ControlPanel',
  props: {
    canStep: {
      type: Boolean,
      default: false
    },
    executionInfo: {
      type: Object,
      default: () => ({
        stepCount: 0,
        currentLine: 0,
        canStepBack: false,
        hasExecution: false
      })
    },
    isCompiling: {
      type: Boolean,
      default: false
    }
  },
  emits: ['step-forward', 'step-backward', 'reset', 'compile']
}
</script>