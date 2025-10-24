# Claude Code Iteration Helper - Guida

## Panoramica

`claude_iterate.py` è uno script helper che automatizza il ciclo build→verify→analyze per la decompilazione di Metal Gear Solid, ottimizzato per l'uso con Claude Code.

## Vantaggi

- **Automazione completa**: Esegue build, verifica hash SHA256, e mostra diff in un unico comando
- **Output strutturato**: Risultati chiari e colorati per identificare rapidamente problemi
- **Watch mode**: Monitora file sorgenti e ricompila automaticamente al salvataggio
- **Integrazione nativa**: Usa gli strumenti esistenti del progetto (build.py, compare.py)
- **Disassembly dettagliato**: Con capstone installato, mostra diff assembly side-by-side

## Installazione Dipendenze

```bash
cd /home/user/mgs_reversing/build
pip3 install -r requirements.txt
```

Verifica installazione:
```bash
python3 claude_iterate.py --help
```

## Utilizzo

### 1. Build Singola (Modalità Standard)

Compila il progetto ed esegue la verifica:

```bash
cd /home/user/mgs_reversing/build
python3 claude_iterate.py
```

Output tipico se **non matcha**:
```
======================================================================
         CLAUDE CODE ITERATION CYCLE - MAIN_EXE
======================================================================

>>> Running build...
✓ Build completed in 45.32s

>>> Verifying build output...
✗ Main executable: MISMATCH
✓ Overlays: 92/92 match

>>> Analyzing differences...
[Disassembly diff di funzioni non matching...]

======================================================================
                         SUMMARY
======================================================================

⚠ BUILD COMPLETED - NEEDS ITERATION

The compiled binary does not match the target.
Review the differences above and iterate on the source code.
```

Output se **matcha perfettamente**:
```
======================================================================
         CLAUDE CODE ITERATION CYCLE - MAIN_EXE
======================================================================

>>> Running build...
✓ Build completed in 45.32s

>>> Verifying build output...
✓ Main executable: MATCH
✓ Overlays: 92/92 match

======================================================================
                         SUMMARY
======================================================================

🎉 BUILD SUCCESSFUL - PERFECT MATCH!

The compiled binary matches the target perfectly.
This code is ready to commit!
```

### 2. Build con Output Verbose

Mostra tutto l'output della compilazione:

```bash
python3 claude_iterate.py --verbose
```

Utile per:
- Debugging errori di compilazione
- Vedere tutti i warning del compiler
- Analizzare il processo di linking

### 3. Watch Mode (Ricompilazione Automatica)

Monitora un file sorgente e ricompila automaticamente quando viene modificato:

```bash
# Esempio: monitorare un file specifico
python3 claude_iterate.py --watch ../source/libgv/memory.c

# Con verbose
python3 claude_iterate.py --watch ../source/game/game.c --verbose
```

Funzionamento:
1. Avvia il monitoring del file
2. Quando salvi modifiche al file, lo script rileva il cambiamento
3. Esegue automaticamente build→verify→analyze
4. Torna in modalità watch
5. Premi `Ctrl+C` per uscire

**Perfetto per iterare velocemente su una singola funzione!**

### 4. Build di Varianti Specifiche

#### VR Executable
```bash
python3 claude_iterate.py --variant=vr_exe
```

#### Dev Executable (per testing in emulatore)
```bash
python3 claude_iterate.py --variant=dev_exe
```

Nota: dev_exe salta la verifica hash (utile per testing rapido).

## Workflow per Claude Code

### Workflow Standard - Decompilare una Funzione

1. **Identifico la funzione** da decompilare (es. da `functions.txt`)

2. **Leggo l'assembly** esistente:
   ```bash
   # Es: funzione in asm/overlays/s11g/blastoff_800D4744.s
   ```

3. **Scrivo/modifico il codice C** in `/source/`

4. **Eseguo verifica**:
   ```bash
   python3 claude_iterate.py
   ```

5. **Analizzo le differenze** mostrate dallo script

6. **Itero** modificando il codice finché non matcha perfettamente

7. **Ripeto** per la prossima funzione

### Workflow Rapido - Con Watch Mode

1. **Avvio watch mode** sul file che sto modificando:
   ```bash
   python3 claude_iterate.py --watch ../source/libgv/memory.c
   ```

2. **Modifico il file** nel mio editor

3. **Salvo** - lo script ricompila automaticamente

4. **Analizzo** l'output nel terminale

5. **Itero** - modifico, salvo, vedo risultati immediatamente

**Zero comandi manuali tra le iterazioni!**

## Esempi Pratici

### Esempio 1: Decompilare una funzione in libgv

```bash
# 1. Leggo l'assembly per capire cosa fa la funzione
cat ../asm/libgv/some_function.s

# 2. Scrivo il codice C
# (modifico ../source/libgv/file.c)

# 3. Test rapido con watch mode
python3 claude_iterate.py --watch ../source/libgv/file.c

# 4. Modifico, salvo, vedo risultati
# 5. Quando matcha, Ctrl+C per uscire
```

### Esempio 2: Verificare una build completa

```bash
# Build completa con dettagli
python3 claude_iterate.py --verbose

# Controlla tutte le funzioni e tutti gli overlay
```

### Esempio 3: Testing VR variant

```bash
# Build e verifica VR executable
python3 claude_iterate.py --variant=vr_exe --verbose
```

## Interpretare l'Output

### Simboli di Status

- `✓` (verde) = Successo / Match perfetto
- `✗` (rosso) = Fallimento / Mismatch
- `ℹ` (giallo) = Informazione / Warning
- `🎉` = Build completamente matching!
- `⚠` = Build completata ma richiede iterazione
- `👁` = Watch mode attivo

### Sezioni Output

1. **Header**: Mostra variante in corso
2. **Running build**: Tempo di compilazione e status
3. **Verifying build output**: Confronto hash SHA256
4. **Analyzing differences**: Diff dettagliato delle funzioni (se mismatch)
5. **Summary**: Verdetto finale e azioni consigliate

## Tips & Tricks

### 1. Watch Mode + Verbose
```bash
python3 claude_iterate.py -w ../source/game/game.c -v
```
Perfetto quando hai errori di compilazione da debuggare.

### 2. Quick Check
```bash
python3 claude_iterate.py
```
Verifica rapida senza verbose quando sei sicuro che compila.

### 3. Focus su Overlay Specifico
Se lavori su un overlay specifico, il sistema verifica automaticamente tutti gli overlay.

### 4. Capstone per Diff Dettagliati
Se capstone è installato (lo è se hai eseguito `pip3 install -r requirements.txt`), vedrai:
- Disassembly side-by-side
- Evidenziazione delle differenze
- Indirizzi e istruzioni formattate

## Risoluzione Problemi

### "ERROR: Could not import compare.py"
Esegui lo script dalla directory `build/`:
```bash
cd /home/user/mgs_reversing/build
python3 claude_iterate.py
```

### "Build FAILED"
Usa `--verbose` per vedere l'output completo:
```bash
python3 claude_iterate.py --verbose
```

### "File not found" in Watch Mode
Verifica il path relativo dal directory `build/`:
```bash
# Corretto
python3 claude_iterate.py --watch ../source/libgv/file.c

# Sbagliato
python3 claude_iterate.py --watch source/libgv/file.c
```

### PSY-Q SDK non trovato
Assicurati che PSY-Q SDK sia in `../../psyq_sdk` o specifica il path:
```bash
export PSYQ_SDK=/path/to/psyq_sdk
python3 claude_iterate.py
```

## Integrazione con Workflow Git

```bash
# 1. Lavora su una funzione
python3 claude_iterate.py --watch ../source/libgv/memory.c

# 2. Quando matcha, esci (Ctrl+C)

# 3. Verifica finale completa
python3 claude_iterate.py

# 4. Se tutto ok, commit
git add ../source/libgv/memory.c
git commit -m "Decompile memory.c function XYZ"
```

## Confronto: decomp.me vs claude_iterate.py

| Feature | decomp.me | claude_iterate.py |
|---------|-----------|-------------------|
| Setup | Browser, nessun setup | Richiede PSY-Q SDK |
| Automazione | Manuale (copia/incolla) | Completamente automatico |
| Claude Code | ❌ Richiede interazione umana | ✅ Claude può iterare da solo |
| Watch Mode | ❌ No | ✅ Si |
| Full Build | ❌ Solo singole funzioni | ✅ Build completa |
| Overlay Verification | ❌ No | ✅ Si |
| Community Sharing | ✅ Si | ❌ No (locale) |
| Velocità iterazione | Media (copia/incolla) | Veloce (automatico) |

**Conclusione**: `claude_iterate.py` è ideale per Claude Code perché permette automazione completa senza intervento umano!

## Next Steps

Dopo aver verificato che tutto funziona:

1. **Setup PSY-Q SDK** (se non già fatto)
2. **Prova una build** con `python3 claude_iterate.py`
3. **Identifica una funzione** da decompilare
4. **Usa watch mode** per iterare rapidamente
5. **Commit** quando ottieni un match perfetto!

---

**Made for Claude Code** 🤖
