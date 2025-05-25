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
    A[Start] --> B[Load GloVe Embeddings]
    B -->|Store in HashMap| C[Load Activities from CSV]
    C -->|Parse Tags| D[Calculate Activity Vectors]
    D -->|Average Tag Embeddings| E[User Input: Group Size]
    E --> F[Collect Preferences for Each Person]
    F -->|Space-separated Keywords| G[Process Keywords]
    G -->|Lowercase, Remove Punctuation| H[Find Embeddings for Keywords]
    H -->|Handle Compound Words| I[Sum Valid Embeddings]
    I -->|Average for Group Vector| J[Compare Group Vector with Activity Vectors]
    J -->|Cosine Similarity| K[Select Top 3 Activities]
    K --> L[Display Recommendations]
    L --> M[Prompt: 'start' or 'quit']
    M -->|start| E
    M -->|quit| N[Cleanup and Exit]

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

#### Using Make

```bash
make
./arc
```

#### Manual Compilation

```bash
gcc -o arc search.c src/utils.c src/colors.c src/embeddings.c src/activities.c -lm
./arc
```

### **Docker**

#### Build Docker Image

```bash
docker build -t arc:latest .
```

#### Run Docker Container

```bash
docker run -it --name arc-container \
  -v ./data:/app/data \
  arc:latest
```

This mounts the local `data` folder to the container, which should contain the required files: `glove.6B.300d.txt` and `activities.csv`.

#### Using Shell Scripts →

```bash
# Build and run with Make
./Shell-Scripts/Make-Command.sh

# Build and run with Docker (includes make compilation)
./Shell-Scripts/Docker.sh

# Delete Docker container
./Shell-Scripts/Delete-Container.sh
```

Make sure to set execute permissions on the scripts:

```bash
chmod +x Shell-Scripts/*.sh
```

`Note:` Running Docker automatically compiles the code during image building, so no need to run Make separately before Docker.

---

# Resources →
> [Makefile](https://makefiletutorial.com/)
