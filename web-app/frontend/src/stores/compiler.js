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
  }
};