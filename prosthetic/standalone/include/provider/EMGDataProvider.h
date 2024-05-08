#pragma once
#include <torch/torch.h>

/**
 * Virtual class describing a source of live or retro-lib EMG data streams
 * collected from 12 discrete electrodes. Each source is responsible for
 * synchronizing data to 1 ksps (this is done to handle low-level API
 * performance for onboard capture sources).
 *
 * Channel specifications are as follows:
 * 1. Flexor Digitorum Superficialis
 * 2. Flexor Pollicis Longus
 * 3. Flexor Carpi Ulnaris
 * 4. Flexor Digitorum Profundus
 * 5. Extensor Digitorum Communis
 * 6. Extensor Digiti Minimi
 * 7. Extensor Indicis Proprius
 * 8. Extensor Pollicis Longus
 * 9. Extensor Pollicis Brevis
 * 10. Extensor Carpi Ulnaris
 * 11. Extensor Carpi Radialis Brevis
 * 12. Extensor Carpi Radialis Longus
 */
class EMGDataProvider {
 public:
  virtual ~EMGDataProvider() = default;

  virtual bool HasData() = 0;

  virtual torch::Tensor GetDataFrame() noexcept(false) = 0;
};
