import express from 'express';
import { CompilerService } from '../services/compilerService.js';

const router = express.Router();
const compilerService = new CompilerService();

// POST /api/compile
router.post('/', async (req, res) => {
  try {
    const { sourceCode, options = {} } = req.body;
    
    if (!sourceCode) {
      return res.status(400).json({
        success: false,
        error: 'sourceCode es requerido'
      });
    }

    console.log('🔨 Compilando código...');
    const result = await compilerService.compileCode(sourceCode, options);
    
    res.json(result);
  } catch (error) {
    console.error('Error en compilación:', error);
    res.status(500).json({
      success: false,
      error: error.message
    });
  }
});

// GET /api/compile/status
router.get('/status', (req, res) => {
  res.json({
    compilerAvailable: compilerService.isAvailable(),
    timestamp: new Date().toISOString()
  });
});

export default router;