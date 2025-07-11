cd arc-server

DIR="data"
URL="https://nlp.stanford.edu/data/glove.6B.zip"
FILE="$DIR/glove.6B.zip"
TARGET="$DIR/glove.6B.300d.txt"

mkdir -p "$DIR"

if [ ! -f "$FILE" ]; then
  echo "Downloading embeddings..."
  curl -L -o "$FILE" "$URL"
fi

if [ ! -f "$TARGET" ]; then
  echo "Extracting glove.6B.300d.txt..."
  unzip -j "$FILE" "glove.6B.300d.txt" -d "$DIR"
fi

rm "$FILE"