import express from 'express';
import cors from 'cors';
import { WebSocketServer } from 'ws';
import { createServer } from 'http';
import compileRoutes from './routes/compile.js';
import executeRoutes from './routes/execute.js';
import inputRoutes from './routes/inputs.js';

const app = express();
const server = createServer(app);
const wss = new WebSocketServer({ server });

// Middleware
app.use(cors());
app.use(express.json());

// Routes
app.use('/api/compile', compileRoutes);
app.use('/api/execute', executeRoutes);
app.use('/api/inputs', inputRoutes);

// Health check
app.get('/api/health', (req, res) => {
  res.json({ status: 'OK', timestamp: new Date().toISOString() });
});

// WebSocket para debugging en tiempo real
wss.on('connection', (ws) => {
  console.log('Cliente conectado para debugging');
  
  ws.on('message', (message) => {
    try {
      const data = JSON.parse(message);
      console.log('Mensaje recibido:', data);
      
      // Broadcast a todos los clientes
      wss.clients.forEach((client) => {
        if (client.readyState === 1) { // WebSocket.OPEN
          client.send(JSON.stringify({
            type: 'debug_update',
            data: data
          }));
        }
      });
    } catch (error) {
      console.error('Error procesando mensaje:', error);
    }
  });

  ws.on('close', () => {
    console.log('Cliente desconectado');
  });
});

const PORT = process.env.PORT || 8000;
server.listen(PORT, () => {
  console.log(`🚀 Servidor backend corriendo en puerto ${PORT}`);
  console.log(`📡 WebSocket server activo`);
});