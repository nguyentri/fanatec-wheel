import os
import sys
import glob

print("Importing libraries...")
from transformers import AutoTokenizer, AutoModelForSeq2SeqLM

# Create output directory if it doesn't exist
in_dir = os.path.join("docs", "studies", "en")
out_dir = os.path.join("docs", "studies", "vn")
os.makedirs(out_dir, exist_ok=True)

print("Loading tokenizer and model...")
tokenizer_en2vi = AutoTokenizer.from_pretrained("vinai/vinai-translate-en2vi-v2", src_lang="en_XX")
model_en2vi = AutoModelForSeq2SeqLM.from_pretrained("vinai/vinai-translate-en2vi-v2")
print("Model loaded.")

def translate_en2vi(en_text: str) -> str:
    if not en_text.strip():
        return ""
    try:
        # Avoid too long sequences
        input_ids = tokenizer_en2vi(en_text, return_tensors="pt", truncation=True, max_length=1024).input_ids
        output_ids = model_en2vi.generate(
            input_ids,
            decoder_start_token_id=tokenizer_en2vi.lang_code_to_id["vi_VN"],
            num_return_sequences=1,
            num_beams=5,
            early_stopping=True,
            max_length=1024
        )
        vi_text = tokenizer_en2vi.batch_decode(output_ids, skip_special_tokens=True)
        return " ".join(vi_text)
    except Exception as e:
        print(f"Error translating chunk: {e}")
        return en_text # fallback to english

def process_file(in_path, out_path):
    print(f"Translating {in_path} -> {out_path}")
    with open(in_path, "r", encoding="utf-8") as f:
        lines = f.readlines()
    
    out_lines = []
    # For markdown, translating line by line is usually safest to preserve formatting.
    # We group text lines until a certain length or process line by line.
    # To keep it simple and robust, process line by line. Blank lines are skipped.
    
    for idx, line in enumerate(lines):
        # Translate only if there's alphanumeric content
        if any(c.isalpha() for c in line):
            # To handle long lines, we could split, but truncation=True will handle it.
            vi_line = translate_en2vi(line.strip())
            # restore trailing newline if it existed
            if line.endswith('\n'):
                vi_line += '\n'
            out_lines.append(vi_line)
        else:
            out_lines.append(line)
        
        if (idx + 1) % 10 == 0:
            print(f"  Processed {idx + 1}/{len(lines)} lines")

    with open(out_path, "w", encoding="utf-8") as f:
        f.writelines(out_lines)
    print(f"Finished {out_path}")

md_files = glob.glob(os.path.join(in_dir, "*.md"))
for i, fpath in enumerate(md_files):
    fname = os.path.basename(fpath)
    out_path = os.path.join(out_dir, fname)
    if os.path.exists(out_path):
        print(f"Skipping {fname}, already translated.")
        continue
    print(f"--- File {i+1}/{len(md_files)} ---")
    process_file(fpath, out_path)

print("All done!")
