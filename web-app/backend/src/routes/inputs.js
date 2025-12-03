import express from 'express';
import fs from 'fs-extra';
import path from 'path';
import { fileURLToPath } from 'url'; // <--- Importar esto

const router = express.Router();

// Definir __dirname manualmente para ES Modules
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

// Ajusta esta ruta según tu estructura real. 
// Basado en CompilerService: projectRoot/core/inputs
const PROJECT_ROOT = path.resolve(__dirname, '../../../../');
const INPUTS_DIR = path.join(PROJECT_ROOT, 'core', 'inputs');

// GET /api/inputs - Listar todos los archivos .txt
router.get('/', async (req, res) => {
  try {
    await fs.ensureDir(INPUTS_DIR);
    const files = await fs.readdir(INPUTS_DIR);
    
    // Filtrar solo .txt y ordenar numéricamente (input1, input2, input10...)
    const txtFiles = files
      .filter(f => 
        f.endsWith('.txt') && 
        f.startsWith('input') && 
        !f.includes('tokens') &&  // <-- IMPORTANTE
        !f.includes('_tokens')    // <-- DOBLE SEGURO
      )
      .sort((a, b) => {
        const numA = parseInt(a.match(/\d+/)?.[0] || 0);
        const numB = parseInt(b.match(/\d+/)?.[0] || 0);
        return numA - numB;
      });

    res.json(txtFiles);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// GET /api/inputs/:filename - Leer contenido
router.get('/:filename', async (req, res) => {
  try {
    const filePath = path.join(INPUTS_DIR, req.params.filename);
    if (!await fs.pathExists(filePath)) {
      return res.status(404).json({ error: 'Archivo no encontrado' });
    }
    const content = await fs.readFile(filePath, 'utf8');
    res.json({ content });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// POST /api/inputs/:filename - Guardar contenido
router.post('/:filename', async (req, res) => {
  try {
    const { content } = req.body;
    const filePath = path.join(INPUTS_DIR, req.params.filename);
    
    await fs.writeFile(filePath, content, 'utf8');
    res.json({ success: true, message: 'Archivo guardado correctamente' });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

export default router;