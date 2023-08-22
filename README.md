```markdown
# DICOM Anonymizer

The DICOM Anonymizer is a c++ command-line tool for anonymizing DICOM files and series using JSON-defined tag values.

## Table of Contents

- [Description](#description)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Dependencies](#dependencies)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Description

The DICOM Anonymizer is a C++ program that uses the Insight Segmentation and Registration Toolkit (ITK) library to read and write DICOM files while anonymizing specified tags. It supports both individual DICOM files and DICOM series stored in a directory.

## Installation

To use the DICOM Anonymizer, follow these steps:

1. Clone or download this repository.
2. Make sure you have the ITK library and JSONCPP installed on your system.
3. Compile the code using your CMAKE 

## Usage

```
Usage: ./anonymizer dicomfilename(foldername) tagkeyfile
```

- `dicomfilename`: The path to a DICOM file or a folder containing a DICOM series.
- `tagkeyfile`: A JSON file containing the tag keys and values to be used for anonymization.

## Configuration

Edit the `tagkeyfile` JSON file to specify the tag keys and values to be anonymized.

Example `tagkeyfile.json`:
```json
{
  "PatientID": "ANONYMIZED",
  "PatientName": "Anonymous Patient",
  "InstitutionName": "Anonymized Institution"
}
```

## Dependencies

- [ITK](https://itk.org/): The Insight Segmentation and Registration Toolkit.
- [JSONCPP](https://github.com/open-source-parsers/jsoncpp): A C++ library for interacting with JSON.

## Contributing

Contributions are welcome! If you find a bug or want to suggest an improvement, please open an issue or submit a pull request.
