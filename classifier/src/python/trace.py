from model import TransformerModel
import torch
import os

def main():
    device = torch.device("cuda" if torch.cuda.is_available() else "mps" if torch.backends.mps.is_available() else "cpu")

    encoder = TransformerModel().to(device)
    encoder.load_state_dict(torch.load(os.path.realpath("classifier/model_weights/weights.pt"), map_location=device))
    encoder.eval()

    traced_encoder = torch.jit.trace(encoder, torch.rand(1, 512, 12).to(device))
    traced_encoder.save(f"classifier/model_weights/traced_model_{device}.pt")

if __name__ == "__main__":
    main()
