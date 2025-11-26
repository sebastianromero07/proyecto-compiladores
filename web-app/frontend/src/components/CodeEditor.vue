<template>
  <div class="h-full flex flex-col bg-black overflow-hidden">
    <div class="bg-gradient-to-r from-gray-900 to-black px-4 py-2 border-b border-cyan-500/30 flex-shrink-0">
      <h3 class="text-sm font-bold text-cyan-400 tracking-wide">SOURCE CODE</h3>
    </div>
    
    <div class="flex-1 overflow-hidden bg-black">
      <div class="h-full flex overflow-hidden bg-black">
        <!-- Line numbers -->
        <div class="bg-gray-900/50 px-3 py-4 border-r border-cyan-500/20 overflow-y-auto flex-shrink-0" style="scrollbar-width: none;">
          <div class="font-mono text-xs text-gray-600 space-y-0.5">
            <div v-for="n in lineCount" :key="n" class="leading-6">{{ n }}</div>
          </div>
        </div>
        
        <!-- Code area -->
        <textarea 
          v-model="localCode"
          @input="handleInput"
          class="flex-1 bg-black text-gray-100 font-mono text-sm p-4 resize-none outline-none border-none leading-6 overflow-y-auto"
          placeholder="Write your C code here..."
          spellcheck="false"
        ></textarea>
      </div>
    </div>

    <!-- Quick Examples -->
    <div class="bg-gradient-to-r from-gray-900 to-black px-4 py-2 border-t border-cyan-500/30 flex-shrink-0">
      <div class="flex gap-2 text-xs items-center">
        <span class="text-gray-500 mr-2 font-mono">EXAMPLES:</span>
        <button 
          v-for="example in examples" 
          :key="example.name"
          @click="loadExample(example)"
          class="bg-gray-800 hover:bg-gray-700 border border-cyan-500/30 hover:border-cyan-500 px-3 py-1 rounded transition-all text-cyan-400 font-mono"
        >
          {{ example.name }}
        </button>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  props: {
    modelValue: String,
    isLoading: Boolean
  },
  emits: ['update:modelValue', 'compile', 'download'],
  data() {
    return {
      localCode: this.modelValue,
      examples: [
        {
          name: 'Basic',
          code: `#include<stdio.h>

int main(){
    int x = 5;
    printf("Hello: %d\\n", x);
    return 0;
}`
        },
        {
          name: 'Function',
          code: `#include<stdio.h>

int sum(int a, int b){
    return a + b;
}

int main(){
    int result = sum(5, 3);
    printf("Result: %d\\n", result);
    return 0;
}`
        },
        {
          name: 'If-Else',
          code: `#include<stdio.h>

int main(){
    int x = 5;
    int y = 10;
    
    if (x > y) {
        printf("x is greater\\n");
    } else {
        printf("y is greater\\n");
    }
    return 0;
}`
        }
      ]
    }
  },
  computed: {
    lineCount() {
      return (this.localCode || '').split('\n').length;
    }
  },
  watch: {
    modelValue(newVal) {
      this.localCode = newVal;
    }
  },
  methods: {
    handleInput() {
      this.$emit('update:modelValue', this.localCode);
    },
    loadExample(example) {
      this.localCode = example.code;
      this.$emit('update:modelValue', this.localCode);
    }
  }
}
</script>

<style scoped>
textarea {
  tab-size: 4;
  caret-color: #06b6d4;
}

textarea::placeholder {
  color: #4b5563;
}

textarea::selection {
  background: rgba(6, 182, 212, 0.3);
}
</style>