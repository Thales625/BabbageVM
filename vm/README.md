# Instruction Set Architecture (ISA) – 16-bit Opcodes

Este documento descreve o formato de instruções de 16 bits, modos de endereçamento e tabela de opcodes do processador.

---

## Formato da Instrução (16 bits)

```
0b0000000000000000
  fedcba9876543210
  OOOOOOOOX*aaX*AA
```

- **O**: Opcode (instrução)
- **A**: Modo de endereçamento do operando 1
- **a**: Modo de endereçamento do operando 2
- **X**: Don’t care (ignorado)
- **\***: Flag reg/mem

Representação em **hexadecimal** = ```0xOOaA```

---

## Modos de Endereçamento

- `X*00` → **REG/MEM direto**
- `X*01` → **REG/MEM indireto**
- `XX10` → **imediato**

### Operando 1 (REG)
- `X000 = 0` → **direto**
- `X001 = 1` → **indireto**
- `X010 = 2` → **imediato**

### Operando 2 (MEM)
- `X100 = 8` → **direto**
- `X101 = 9` → **indireto**
- `X110 = a` → **imediato**

---

## Estrutura do Opcode

- `8-F` → Opcode (**instrução**) `<byte>`
- `0-2` → Modo de endereçamento do **operando 1**
- `4-6` → Modo de endereçamento do **operando 2**

---

## Código Mnemônico (Exemplos)

```asm
add  %x   ;  acc += reg[x]  ;  registrador direto
add  [x]  ;  acc += mem[x]  ;  memoria direto

add  ,I%x   ;  acc += reg[reg[x]]  ;  registrador indireto
add  ,I[x]  ;  acc += mem[mem[x]]  ;  memoria indireto

add ,Ix  ;  acc += mem[x]  ;  indireto
add  #x  ;  acc += v       ;  imediato
```

---

## Tabela de Instruções (Opcode)

| Opcode | Mnemonic | Operandos        |
|--------|----------|------------------|
| `00`   | BR       | (opd_1)          |
| `01`   | BRPOS    | (opd_1)          |
| `02`   | ADD      | (opd_1)          |
| `03`   | LOAD     | (opd_1)          |
| `04`   | BRZERO   | (opd_1)          |
| `05`   | BRNEG    | (opd_1)          |
| `06`   | SUB      | (opd_1)          |
| `07`   | STORE    | (opd_1)          |
| `10`   | DIVIDE   | (opd_1)          |
| `11`   | STOP     | -                |
| `13`   | COPY_R   | (opd_1, opd_2)   |
| `14`   | MULT     | (opd_1)          |
| `15`   | CALL     | (opd_1)          |
| `16`   | RET      | -                |
| `17`   | PUSH     | (opd_1)          |
| `18`   | POP      | (opd_1)          |
| `19`   | COPY_M   | (opd_1, opd_2)   |

---

## Resumo

- **Instruções de 16 bits**
- **Opcode (8 bits) + modos de endereçamento**
- Suporte a **direto, indireto e imediato**
- Conjunto de instruções para controle de fluxo, aritmética e manipulação de memória
