import { spawn } from 'child_process';
import fs from 'fs-extra';
import path from 'path';

export class CompilerService {
  constructor() {
    this.projectRoot = path.resolve('../../'); // Root del proyecto
    this.coreDir = path.join(this.projectRoot, 'core');
    this.inputsDir = path.join(this.coreDir, 'inputs');      // ✅ inputs está en core/
    this.outputsDir = path.join(this.coreDir, 'outputs');    // ✅ outputs está en core/
    
    console.log('📁 Directorios configurados:');
    console.log('  Project Root:', this.projectRoot);
    console.log('  Core:', this.coreDir);
    console.log('  Inputs:', this.inputsDir);
    console.log('  Outputs:', this.outputsDir);
  }

  async compileCode(sourceCode, options = {}) {
    try {
      // ✅ Asegurar que las carpetas existan
      await fs.ensureDir(this.inputsDir);
      await fs.ensureDir(this.outputsDir);
      
      // 1. Crear archivo temporal
      const timestamp = Date.now();
      const inputFile = path.join(this.inputsDir, `web_input_${timestamp}.txt`);
      await fs.writeFile(inputFile, sourceCode);
      
      console.log(`📝 Archivo creado: ${inputFile}`);

      // 2. Verificar que el compilador existe
      const compilerPaths = [
        path.join(this.coreDir, 'a.out'),
        path.join(this.coreDir, 'compiler'),
        path.join(this.coreDir, 'compiler.exe'),
        path.join(this.coreDir, 'a.exe')
      ];
      
      let compilerPath = null;
      for (const p of compilerPaths) {
        if (await fs.pathExists(p)) {
          compilerPath = p;
          console.log(`✅ Compilador encontrado: ${compilerPath}`);
          break;
        }
      }
      
      if (!compilerPath) {
        // Listar archivos en core para debug
        const coreFiles = await fs.readdir(this.coreDir);
        console.log('📁 Archivos en core:', coreFiles);
        throw new Error(`❌ Compilador no encontrado en: ${this.coreDir}\nArchivos disponibles: ${coreFiles.join(', ')}`);
      }

      // 3. Ejecutar compilador
      const result = await this.runCompiler(compilerPath, inputFile);

      // 4. Procesar resultados
      if (result.success) {
        // ✅ CORREGIDO: Buscar assembly en inputs/ (dentro de core/)
        const assemblyFile = path.join(this.inputsDir, `web_input_${timestamp}.s`);
        
        let assembly = '';
        if (await fs.pathExists(assemblyFile)) {
          assembly = await fs.readFile(assemblyFile, 'utf8');
          console.log(`📄 Assembly encontrado: ${assemblyFile}`);
          
          // Mover a outputs/ (dentro de core/)
          const destFile = path.join(this.outputsDir, `web_input_${timestamp}.s`);
          await fs.move(assemblyFile, destFile);
          console.log(`📁 Assembly movido a: ${destFile}`);
        } else {
          console.log(`⚠️ No se encontró archivo assembly en: ${assemblyFile}`);
        }

        return {
          success: true,
          tokens: result.tokens,
          assembly: assembly,
          ast: result.ast,
          executionSteps: result.steps,
          timestamp: timestamp,
          compilerOutput: result.output
        };
      }

      return result;
    } catch (error) {
      console.error('❌ Error en compilación:', error);
      return {
        success: false,
        error: error.message
      };
    }
  }

  async runCompiler(compilerPath, inputFile) {
    return new Promise((resolve) => {
      console.log(`🔨 Ejecutando: ${compilerPath}`);
      console.log(`📂 Input file: ${inputFile}`);
      console.log(`📂 Working dir: ${this.coreDir}`);
      
      // ✅ IMPORTANTE: Usar ruta relativa desde core/
      const relativeInputFile = path.relative(this.coreDir, inputFile);
      console.log(`📂 Relative path: ${relativeInputFile}`);
      
      const process = spawn(compilerPath, [relativeInputFile], {
        cwd: this.coreDir,  // Ejecutar desde core/
        stdio: ['pipe', 'pipe', 'pipe']
      });
      
      let stdout = '';
      let stderr = '';

      process.stdout.on('data', (data) => {
        stdout += data.toString();
      });

      process.stderr.on('data', (data) => {
        stderr += data.toString();
      });

      process.on('close', (code) => {
        console.log(`🏁 Compilador terminó con código: ${code}`);
        console.log('📤 STDOUT length:', stdout.length);
        console.log('📤 STDOUT preview:', stdout.substring(0, 200));
        if (stderr) {
          console.log('📤 STDERR:', stderr.substring(0, 200));
        }

        resolve({
          success: code === 0,
          tokens: this.parseTokens(stdout),
          ast: this.parseAST(stdout),
          steps: this.parseExecutionSteps(stdout),
          output: stdout,
          error: stderr
        });
      });

      process.on('error', (error) => {
        console.error('❌ Error ejecutando compilador:', error);
        resolve({
          success: false,
          error: `Error ejecutando compilador: ${error.message}`
        });
      });
    });
  }

  parseTokens(output) {
    // Extraer tokens de la salida de tu compilador
    const lines = output.split('\n');
    const tokens = [];
    
    for (const line of lines) {
      const trimmedLine = line.trim();
      // Buscar diferentes formatos de tokens que podría generar tu compilador
      if (trimmedLine.includes('TOKEN(') || 
          trimmedLine.includes('Token:') || 
          trimmedLine.includes('token') ||
          (trimmedLine.includes('ID:') || trimmedLine.includes('NUM:') || trimmedLine.includes('OP:'))) {
        tokens.push(trimmedLine);
      }
    }
    
    console.log(`🏷️ Tokens extraídos: ${tokens.length}`);
    if (tokens.length > 0) {
      console.log('🏷️ Primer token:', tokens[0]);
    }
    
    return tokens;
  }

  parseAST(output) {
    // Extraer información del AST si tu compilador la genera
    const astLines = output.split('\n').filter(line => 
      line.includes('AST') || 
      line.includes('Node') || 
      line.includes('Expression') ||
      line.includes('Statement')
    );
    
    return { 
      message: 'AST parsing implementado',
      nodes: astLines,
      hasAST: astLines.length > 0
    };
  }

  parseExecutionSteps(output) {
    // Extraer pasos de ejecución del intérprete
    const lines = output.split('\n');
    const steps = [];
    
    for (const line of lines) {
      const trimmed = line.trim();
      if (trimmed.includes('=') || 
          trimmed.includes('printf') || 
          trimmed.includes('->') ||
          trimmed.includes('Executing') ||
          trimmed.includes('Result:')) {
        steps.push({
          line: trimmed,
          timestamp: new Date().toISOString()
        });
      }
    }
    
    console.log(`⚡ Execution steps: ${steps.length}`);
    return steps;
  }

  async isAvailable() {
    const compilerPaths = [
      path.join(this.coreDir, 'a.out'),
      path.join(this.coreDir, 'compiler'),
      path.join(this.coreDir, 'compiler.exe'),
      path.join(this.coreDir, 'a.exe')
    ];
    
    for (const p of compilerPaths) {
      if (await fs.pathExists(p)) {
        console.log(`✅ Compilador disponible: ${p}`);
        return true;
      }
    }
    
    console.log('❌ No se encontró compilador en ninguna ubicación');
    return false;
  }

  // ✅ NUEVO: Método para diagnosticar el entorno
  async diagnose() {
    const diagnosis = {
      projectRoot: this.projectRoot,
      coreDir: this.coreDir,
      inputsDir: this.inputsDir,
      outputsDir: this.outputsDir,
      compilerAvailable: await this.isAvailable(),
      directories: {},
      files: {}
    };

    // Verificar directorios
    diagnosis.directories.coreExists = await fs.pathExists(this.coreDir);
    diagnosis.directories.inputsExists = await fs.pathExists(this.inputsDir);
    diagnosis.directories.outputsExists = await fs.pathExists(this.outputsDir);

    // Listar archivos
    try {
      if (diagnosis.directories.coreExists) {
        const coreFiles = await fs.readdir(this.coreDir);
        diagnosis.files.core = coreFiles;
      }
    } catch (e) {
      diagnosis.files.coreError = e.message;
    }

    return diagnosis;
  }
}