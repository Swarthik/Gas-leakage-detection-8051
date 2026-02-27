# Gas Leakage Detection System 🔴
> BEC405A — Microcontrollers 8051 | Jyothy Institute of Technology

A real-time gas leakage detection system built using the **AT89C51 (8051)
microcontroller**, **MQ-2 gas sensor**, and **ADC0808**. Detects LPG/methane
leaks and instantly alerts via buzzer, LED, and 16x2 LCD display.

---

## 📌 Features
- Real-time gas concentration monitoring
- Visual alert on 16x2 LCD (SAFE / GAS DETECTED)
- Audio + LED alert on threshold breach
- Simulated in Proteus before hardware deployment
- Low-cost and beginner-friendly design

---

## 🔧 Hardware Components
| Component            | Quantity | Cost (₹) |
|----------------------|----------|-----------|
| AT89C51 Microcontroller | 1     | 80        |
| MQ-2 Gas Sensor      | 1        | 120       |
| LCD Display (16x2)   | 1        | 170       |
| Passive Buzzer       | 1        | 10        |
| Breadboard & Wires   | 1 set    | 100       |
| Arduino Uno (power)  | 1        | 450       |
| **Total**            |          | **₹930**  |

---

## 📐 Block Diagram
```
MQ-2 Sensor → ADC0808 → 8051 Microcontroller
                              ↓         ↓        ↓
                           16x2 LCD   Buzzer    LED
```

---

## 🗂️ Repository Structure
```
gas-leakage-detection-8051/
├── src/                  # Embedded C source code
├── simulation/           # Proteus files + HEX
├── hardware/             # Circuit & block diagrams
├── docs/                 # Project report & BOM
└── images/               # Hardware & simulation photos
```

---

## ⚙️ Pin Configuration
| Signal        | Pin     |
|---------------|---------|
| ADC Data Out  | Port 0  |
| LCD Data      | Port 1  |
| LCD RS/RW/EN  | P2.0–P2.2 |
| ADC ALE/START/EOC/OE | P2.4–P2.7 |
| LED Alert     | P3.6    |
| Buzzer Alert  | P3.7    |

---

## 🚀 How to Run
1. Open `simulation/gas_leakage.pdsprj` in **Proteus**
2. Load `simulation/gas_leakage.hex` into the AT89C51
3. Run simulation — adjust MQ-2 sensor value above `100` to trigger alert

To recompile:
1. Open `src/gas_leakage_8051.c` in **Keil µVision**
2. Set target to AT89C51
3. Build → generates new `.hex` file

---

## 📊 Test Results
| Test | Condition       | LCD Output   | Buzzer/LED |
|------|-----------------|--------------|------------|
| 1    | Clean air       | SAFE         | OFF        |
| 2    | Near LPG lighter| GAS DETECTED | ON         |
| 3    | Ventilated area | SAFE         | OFF        |

---

## 👥 Team
| Name            | USN         | Role                        |
|-----------------|-------------|-----------------------------|
| Bhavani B       | IJT23EC019  | Circuit Design, Hardware    |
| Ananya Deepak   | IJT23EC014  | Components, Research        |
| B Swarthik      | IJT23EC016  | Coding, Simulation          |
| Deeksha S       | IJT23EC026  | Documentation, Presentation |

**Guide:** Dayananda L N, Assistant Professor, Dept. of ECE, JIT

---

## 📚 References
- Mazidi, M.A. et al., *The 8051 Microcontroller and Embedded Systems*
- [Keil MDK](https://www.keil.com)
- TutorialsPoint: Interfacing MQ2 with 8051

---

*Jyothy Institute of Technology, Bangalore | Academic Year 2024–25*
