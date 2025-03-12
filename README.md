# Activity Recommender CLI (ARC) 
[Under development]

## Project Overview

**Activity Recommender CLI (ARC)** is a command-line interface (CLI) tool designed to recommend group activities based on collective user preferences. ARC allows multiple users to input keyword tags representing their interests, processes these inputs using word embeddings, and suggests a single activity aligned with the group’s shared preferences. The tool aims to facilitate group decision-making by providing a consensus-driven recommendation through a lightweight, terminal-based application.


### Core Functionality

- **Input Collection**: Captures the number of group members and their keyword tags via command-line input.  
- **Embedding Lookup**: Searches for each keyword in the pre-trained GloVe word embedding file (`glove.6B.50d.txt`) and retrieves the corresponding 50-dimensional vector if available.  
- **Activity Matching**: Reads `activities.csv`, extracts predefined activity tags, retrieves their corresponding vectors from GloVe, and compares them with the input keyword vectors using cosine similarity.  
- **Recommendation Output**: Identifies the top 3 activities with the highest similarity scores and recommends them for the group.  


### Instructions for Downloading and Setting Up GloVe Embeddings 

- [🔗 Click here](https://nlp.stanford.edu/data/glove.6B.zip)  to download the GloVe embeddings
- Extract the ZIP file.  
- Locate `glove.6B.50d.txt`.  
- Move `glove.6B.50d.txt` to the `data` folder.  