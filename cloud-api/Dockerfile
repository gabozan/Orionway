FROM python:3.10-slim

ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /app

COPY requirements.txt ./
COPY app/ ./
COPY models/ ./models/
COPY yolov5/ ./yolov5/

ENV PYTHONPATH="${PYTHONPATH}:/app/yolov5"

RUN pip install --upgrade pip setuptools wheel

RUN apt-get update && apt-get install -y --no-install-recommends \
    libglib2.0-0 libsm6 libxext6 libxrender1 ffmpeg \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN pip install --no-cache-dir -r requirements.txt

ENV PORT=8080

CMD ["gunicorn", "--bind", "0.0.0.0:8080", "app:app"]