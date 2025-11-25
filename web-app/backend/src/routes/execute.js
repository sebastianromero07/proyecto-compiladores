import express from 'express';
import { DebuggerService } from '../services/debuggerService.js';

const router = express.Router();
const debuggerService = new DebuggerService();

// POST /api/execute/start
router.post('/start', async (req, res) => {
  try {
    const { sourceCode } = req.body;
    
    const result = await debuggerService.startExecution(sourceCode);
    res.json(result);
  } catch (error) {
    res.status(500).json({
      success: false,
      error: error.message
    });
  }
});

// POST /api/execute/step
router.post('/step', async (req, res) => {
  try {
    const { direction = 'forward' } = req.body;
    
    const result = direction === 'forward' 
      ? await debuggerService.stepForward()
      : await debuggerService.stepBack();
      
    res.json(result);
  } catch (error) {
    res.status(500).json({
      success: false,
      error: error.message
    });
  }
});

// GET /api/execute/state
router.get('/state', (req, res) => {
  const state = debuggerService.getCurrentState();
  res.json(state);
});

export default router;