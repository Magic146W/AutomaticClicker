<a name="readme-top"></a>
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)](https://github.com/Magic146W/Qt_FileExplorer/blob/master/LICENSE)

<br />
<div align="center">
  <h1 align="center">C++ based Automatic Mouse Clicker</h1>

Welcome to Mini Mouse Manager (MMM), a mouse automatic clicker software developed using Qt 6.6.1 and C++ and built with CMake. MMM is a tool created as a learning project, intended to streamline clicking automation on the Windows platform.<br>


  <p align="center">
    This README provides an initial overview of the mouse automation software. More comprehensive and detailed information can be found in the documentation.
    <br />
  </p>
</div>

<details>
  <summary><h2>Table of Contents</h2></summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#getting-project-to-run-on-visual-studio">Getting project to run on Visual Studio</a></li>
        <li><a href="#getting-project-to-run-on-qt-creator">Getting project to run on Qt Creator</a></li>
      </ul>
    </li>
    <li>
     <a href="#usage">Usage</a>
      <ul>
        <li><a href="#presentation">Presentation</a></li>
      </ul>
    </li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
  </ol>
</details>

## About The Project

Mini Mouse Manager (MMM) is an easy-to-use mouse automatization software. With its simple interface and customizable settings, MMM makes automating clicks a breeze for various tasks. With key features such as:
- Versatility: MMM offers a broad range of functionalities, enabling users to automate various clicking actions, meeting diverse needs and scenarios.
- Control and Flexibility: Users wield precise control over clicking behavior through configurable settings, allowing personalized automation. Utilizing the Windows.h library, MMM provides enhanced control and direct interaction with Windows functionalities.
- Intuitive Interface: MMM features a user-friendly interface that simplifies configuration processes, ensuring a seamless and straightforward user experience.
- Platform Compatibility: Specifically tailored for Windows, MMM utilizes the Windows.h library to guarantee consistent and reliable performance, leveraging native functionalities for efficient clicking automation.
- Enhanced Functionality: MMM facilitates various clicking actions, including auto-clicking at the cursor's location, setting and clicking at specified coordinates, defining user-specific clicking area, and configuring fixed or randomized time intervals between clicks. Users can choose continuous clicking until manually stopped or set a defined number of repetitions for automated tasks.

### Built With

[![Qt](https://img.shields.io/badge/Qt-%23217346.svg?style=for-the-badge&logo=Qt&logoColor=white)](https://www.qt.io/download-open-source) Used as the primary Integrated Development Environment (IDE) for coding the AutomaticClicker project. Version 6.6.1.
<br>

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Getting Started

For the project to function properly, user must have either Qt Creator or Visual Studio IDE 2019 (or newer) installed on their system. These Integrated Development Environments (IDEs) provide the necessary tools and environment to compile, run, and work with the Qt_FileExplorer application seamlessly.

### Prerequisites

**QT**<br>
You will need Qt version 6.6.1 or newer, along with the Developer and Designer Tools. If you're unsure whether these tools are installed or if they are not installed, open the Qt Maintenance Tool by navigating to:
"Tools > Qt Maintenance Tool > Start Maintenance Tool" (select the necessary options in the 'Select Components' tab)
<br>

**Visual Studio**<br>
If you prefer using the Visual Studio IDE, ensure that you have the necessary components installed. Access the Visual Studio Installer, modify your Visual Studio IDE, and search for the 'C++ CMake tools for Windows' in the 'Individual Components' tab. Install it if needed. Alternatively, you can install the C++ Windows package, ensuring to select CMake if you haven't installed C++ yet.
    
### Getting project to run on Visual Studio

1. Navigate "File > Clone Repository..." and select folder for repository <a href="https://github.com/Magic146W/AutomaticClicker.git">https://github.com/Magic146W/AutomaticClicker.git</a>.

2. Setting Up CMake for Visual Studio:
- Open Visual Studio and go to “File &gt; Open &gt; CMake…” and select CMakeLists.txt inside your working directory<br>
  - If you don’t find the CMake option, please go through prerequisites for VS<br>
  -  The first time you open this option, an overview of CMake in Visual Studio will appear. Once done, repeat the process of opening CMake from "File &gt; Open" and select your working directory, then choose CMakeLists.txt.<br>
-  Wait for the project to build.
3. To run the project, either click the green arrow with "Qt Application" written next to it or press F5.


### Getting project to run on Qt Creator

1. Clone the repository to your local machine:
   ```sh
   git clone https://github.com/Magic146W/AutomaticClicker.git
   ```
2. Open the project by clicking on CMakeLists.txt inside of the cloned directory.
3. When opening the project for the first time or if not previously configured, select the kit "Desktop Qt 6.6.1 MSVC2019 64bit" or a newer version and click on "Configure Project."
4. Wait for the initial project setup and build to finish.
5. Run the project by clicking the green arrow or using the shortcut Ctrl+R.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Usage

 **Customization:**
 
- Choose various clicking actions using the provided radio buttons.
- Specify fixed or random time intervals in the described text fields. Utilize special time input fields for random intervals.
- Fill in the fields as needed; only selected settings will be applied.
- Access the hotkey dialog by clicking the “HOTKEY” button to modify the start/stop keyboard signal.
- Use the “SET LOCATION” button to define mouse cursor coordinates or input area size in pixels.
- Utilize the "SAVE" and "LOAD" buttons to preserve your layout, settings, and selected hotkey.

  **Initiating Actions:**

- After configuring settings, initiate the automated clicking process by clicking "Start" or using the specified hotkey.
  
 **Hotkey Usage:**
  
- Employ the specified hotkey for quick access and action initiation. This method is especially useful for swiftly halting the program, particularly when the mouse is clicking every 10 milliseconds.
  
**Caution: The hotkey can be triggered even when the software is minimized and another window is selected due to the software's use of the hook system from the windows.h library.**

### Presentation

<br>

To be filled in...
 
<br>
<br>


 
<br>
<br>
<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Contributing

Contributions make the open-source community a fantastic space to learn, inspire, and create. Your contributions are **very much appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/YourFeature`)
3. Commit your Changes (`git commit -m 'Add my YourFeature'`)
4. Push to the Branch (`git push origin feature/YourFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## License

Distributed under the LGPL v3 License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



