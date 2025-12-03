import axios from 'axios';

const API_BASE = '/api';

export const compilerStore = {
  async compile(sourceCode) {
    try {
      const response = await axios.post(`${API_BASE}/compile`, {
        sourceCode: sourceCode
      });

      return response.data;
    } catch (error) {
      throw new Error(error.response?.data?.error || 'Error de compilación');
    }
  },

  async startExecution(assemblyCode) {
    try {
      const response = await axios.post(`${API_BASE}/execute/start`, {
        sourceCode: assemblyCode
      });
      return response.data;
    } catch (error) {
      throw new Error(error.response?.data?.error || 'Error iniciando ejecución');
    }
  },

  async stepForward() {
    try {
      const response = await axios.post(`${API_BASE}/execute/step`, {
        direction: 'forward'
      });

      return response.data;
    } catch (error) {
      throw new Error(error.response?.data?.error || 'Error en step forward');
    }
  },

  async stepBackward() {
    try {
      const response = await axios.post(`${API_BASE}/execute/step`, {
        direction: 'backward'
      });

      return response.data;
    } catch (error) {
      throw new Error(error.response?.data?.error || 'Error en step backward');
    }
  },

  async getExecutionState() {
    try {
      const response = await axios.get(`${API_BASE}/execute/state`);
      return response.data;
    } catch (error) {
      throw new Error('Error obteniendo estado de ejecución');
    }
  },

  async listInputs() {
    try {
      const response = await axios.get(`${API_BASE}/inputs`);
      return response.data;
    } catch (error) {
      console.error('Error listando inputs:', error);
      return [];
    }
  },

  
  async loadInput(filename) {
    try {
      const response = await axios.get(`${API_BASE}/inputs/${filename}`);
      return response.data.content;
    } catch (error) {
      throw new Error('Error cargando archivo');
    }
  },

  async saveInput(filename, content) {
    try {
      await axios.post(`${API_BASE}/inputs/${filename}`, { content });
      return true;
    } catch (error) {
      throw new Error('Error guardando archivo');
    }
  }


};