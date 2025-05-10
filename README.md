# **Activity Recommender CLI (ARC)**

**Activity Recommender CLI (ARC)** recommends group activities based on **user preferences**. Multiple users input **keywords** representing their interests, which are processed using **word embeddings** to suggest activities matching the group's preferences.

---

![ARC](./demo/sample.png)

---

## **Overview →**

ARC uses **GloVe word embeddings** to analyze group preferences and recommend the most relevant activities. It's a **command-line program built in C**, perfect for quick interactions and lightweight environments.

---

## **How It Works →**

```mermaid
graph TD
    A[Start] --> B[Load Embeddings File]
    B --> C{File Opened?}
    C -- No --> D[Print Error and Exit]
    C -- Yes --> E[Allocate Memory for Line Buffer]
    E --> F{Memory Allocation Success?}
    F -- No --> D
    F -- Yes --> G[Read File Line by Line]
    G --> H{End of File?}
    H -- Yes --> I[Print Progress and Close File]
    H -- No --> J[Parse Word and Vector]
    J --> K{Valid Vector?}
    K -- No --> G
    K -- Yes --> L[Normalize Vector and Add to HashMap]
    L --> G
    I --> M[Load Activities File]
    M --> N{File Opened?}
    N -- No --> O[Print Error and Exit]
    N -- Yes --> P[Allocate Memory for Activities]
    P --> Q{Memory Allocation Success?}
    Q -- No --> O
    Q -- Yes --> R[Read File Line by Line]
    R --> S{End of File?}
    S -- Yes --> T[Close File]
    S -- No --> U[Parse Activity and Tags]
    U --> V[Check if Tags Exist in Embeddings]
    V --> W{Valid Tag Found?}
    W -- No --> X[Process Tags Individually]
    W -- Yes --> Y[Add Vector to Activity]
    X --> Z[Check if Compound Words]
    Z --> AA[Split and Process Words]
    AA --> Y
    Y --> R
    T --> AB[Calculate Activity Vectors]
    AB --> AC[Initialize Average Vector]
    AC --> AD[Process Each Tag]
    AD --> AE{Valid Vector for Tag?}
    AE -- No --> AF[Skip Tag]
    AE -- Yes --> AG[Add Vector to Average]
    AG --> AD
    AF --> AH[Check for Multiple Words]
    AH --> AI[Split and Process Words]
    AI --> AG
    AG --> AJ[Final Average Calculation]
    AJ --> AB
    AJ --> AK[Group Preferences]
    AK --> AL[Ask Group Size]
    AL --> AM[Process Preferences for Each Person]
    AM --> AN[Process Each Word]
    AN --> AO{Word in Embeddings?}
    AO -- No --> AP[Skip Word]
    AO -- Yes --> AQ[Add Word Vector to Group]
    AQ --> AM
    AP --> AN
    AP --> AO
```

1. **Collect** keywords from each group member
2. **Retrieve** vectors of the user input words from **GloVe embeddings**
3. **Sum and average** to get a **group vector** representing overall preferences
4. **Compare** the group vector with **each activity vector** from the CSV database using **cosine similarity**
5. **Output** the **top 3 matching activities** with the **highest similarity scores**

---

## **Features →**

| **Feature**                    | **Description**                                             |
| ------------------------------ | ----------------------------------------------------------- |
| **Multi-user input**           | Collects preferences from multiple group members            |
| **Word embedding-based logic** | Uses semantic meaning for accurate activity recommendation|

---

## **Setup Guide →**

### **Installation**

```bash
git clone https://github.com/Sabique-Islam/Activity-Recommender-CLI
cd Activity-Recommender-CLI
```

### **Download Embeddings**

* **[Click Here to Download GloVe Embeddings](https://nlp.stanford.edu/data/glove.6B.zip)**
* **Extract** the zip
* Move `glove.6B.300d.txt` into the `data/` directory

### **Build & Run**

```bash
gcc -o arc search.c src/utils.c -lm
./arc
```

---
