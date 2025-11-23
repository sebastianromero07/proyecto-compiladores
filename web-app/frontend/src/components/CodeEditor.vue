<template>
  <div class="h-full flex flex-col">
    <div class="bg-slate-800 p-3 border-b border-slate-700 flex justify-between items-center">
      <h3 class="font-bold">📝 Editor de Código C</h3>
      <button 
        @click="$emit('compile')"
        :disabled="isLoading"
        class="bg-blue-600 hover:bg-blue-700 disabled:bg-gray-600 px-4 py-2 rounded transition-colors"
      >
        {{ isLoading ? '⏳ Compilando...' : '🔨 Compilar' }}
      </button>
    </div>
    
    <div class="flex-1">
      <textarea 
        v-model="localCode"
        @input="$emit('update:modelValue', localCode)"
        class="w-full h-full bg-slate-900 text-white font-mono text-sm p-4 resize-none outline-none border-none"
        placeholder="Escribe tu código C aquí..."
      ></textarea>
    </div>

    <!-- Ejemplos rápidos -->
    <div class="bg-slate-800 p-2 border-t border-slate-700">
      <div class="flex gap-2 text-xs">
        <button 
          v-for="example in examples" 
          :key="example.name"
          @click="loadExample(example)"
          class="bg-slate-700 hover:bg-slate-600 px-2 py-1 rounded transition-colors"
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
  emits: ['update:modelValue', 'compile'],
  data() {
    return {
      localCode: this.modelValue,
      examples: [
        {
          name: 'Básico',
          code: `#include<stdio.h>

int main(){
    int x = 5;
    printf("Hola: %d\\n", x);
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
        printf("x es mayor\\n");
    } else {
        printf("y es mayor\\n");
    }
    return 0;
}`
        },
        {
          name: 'Función',
          code: `#include<stdio.h>

int suma(int a, int b){
    return a + b;
}

int main(){
    int result = suma(5, 3);
    printf("Resultado: %d\\n", result);
    return 0;
}`
        }
      ]
    }
  },
  watch: {
    modelValue(newVal) {
      this.localCode = newVal;
    }
  },
  methods: {
    loadExample(example) {
      this.localCode = example.code;
      this.$emit('update:modelValue', this.localCode);
    }
  }
}
</script>