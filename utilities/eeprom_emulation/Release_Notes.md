---
pagetitle: Release Note for EEPROM Emulation SW Pack
lang: en
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

<center>
# Release Notes for <mark>EEPROM Emulation</mark> SW Pack
</center>

# Purpose

STM32Cube enables developers to achieve design success. With a comprehensive suite of professional development tools and embedded software components, STM32Cube allows developers to differentiate products, streamline design cycles, and reduce costs. STM32Cube ecosystem supports all design steps, including selection, configuration, development, debugging, programming, and monitoring.
The STM32Cube embedded software offer provides ready-to-use software components that can be added to a project. It includes STM32 peripheral driver APIs with two levels of abstraction, middleware, board drivers, and examples. There are several distribution channels, including the STM32CubeMX2 tool, the ST website, and GitHub. All embedded software comes with enhanced online documentation, with flowcharts and user sequences.

More documentation is available at [EEPROM Emulation SW Pack online documentation](https://dev.st.com/stm32cube-docs/utilities-eeprom-emulation/latest/en/index.html).

:::

::: {.col-sm-12 .col-lg-8}
# Update history

::: {.collapse}
<input type="checkbox" id="collapse-section-2.2.0" checked aria-hidden="true">
<label for="collapse-section-2.2.0" aria-hidden="true">__2.2.0 / 27-August-2026__</label>
<div>

## Main changes

- Add progressive cleanup with time-bounded execution.
- Add FLITF flash interface support.
- fix reported status of EE_ITF_ErrorCallback() for the FLITF_EDATA flash interface.
\

## Contents

- Software package
  - Core source code and headers
  - EEPROM emulation algorithms (FLITF and NVM)
  - Flash interface (ready-to-use implementation and template)
  - CRC interface (ready-to-use implementation and template)
  - ECC interface (ready-to-use implementation and template)
  - STM32CubeMX2 configuration and code generation assets
\

## Supported devices and boards

- STM32C5 series.
- STM32G5 series.
- STM32H7P series.
- STM32V8 series.
\

## Known Limitations

None.
\

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V9.60.3 + ST-LINK
- MDK-ARM Keil uVision V5.42
- STM32CubeIDE for Visual Studio Code (GCC13 compiler)
- STM32CubeMX2 version 1.1.1
\

## Backward compatibility

None.
\

## Dependencies

None.
\

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section-2.1.1" aria-hidden="true">
<label for="collapse-section-2.1.1" aria-hidden="true">__2.1.1 / 02-July-2026__</label>
<div>

## Main changes

- Update the PDSC file with the CubeMX APIs used by the JSON and JS files.
- Update the STM32CubeMX2 configuration and code generation support.
\

## Contents

- Software package
  - Core source code and headers
  - EEPROM emulation algorithms (FLITF and NVM)
  - Flash interface (ready-to-use implementation and template)
  - CRC interface (ready-to-use implementation and template)
  - ECC interface (ready-to-use implementation and template)
  - STM32CubeMX2 configuration and code generation assets
\

## Supported devices and boards

- STM32C5 series.
\

## Known Limitations

None.
\

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V9.60.3 + ST-LINK
- MDK-ARM Keil uVision V5.42
- STM32CubeIDE for Visual Studio Code (GCC13 compiler)
- STM32CubeMX2 version 1.1.0
\

## Backward compatibility

None.
\

## Dependencies

None.
\

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section-2.1.0" aria-hidden="true">
<label for="collapse-section-2.1.0" aria-hidden="true">__2.1.0 / 12-June-2026__</label>
<div>

## Main changes

- Add support for STM32CubeMX2 configuration and code generation.
\

## Contents

- Software package
  - Core source code and headers
  - EEPROM emulation algorithms (FLITF and NVM)
  - Flash interface (ready-to-use implementation and template)
  - CRC interface (ready-to-use implementation and template)
  - ECC interface (ready-to-use implementation and template)
  - STM32CubeMX2 configuration and code generation assets
\

## Supported devices and boards

- STM32C5 series.
\

## Known Limitations

None.
\

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V9.60.3 + ST-LINK
- MDK-ARM Keil uVision V5.42
- STM32CubeIDE for Visual Studio Code (GCC13 compiler)
- STM32CubeMX2 version 1.0.1
\

## Backward compatibility

None.
\

## Dependencies

None.
\

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section-2.0.0" aria-hidden="true">
<label for="collapse-section-2.0.0" aria-hidden="true">__2.0.0 / 13-March-2026__</label>
<div>

## Main changes

- First official release of **EEPROM Emulation**.
\

## Contents

- Software package
  - Core source code and headers
  - EEPROM emulation algorithms (FLITF and NVM)
  - Flash interface (ready-to-use implementation and template)
  - CRC interface (ready-to-use implementation and template)
  - ECC interface (ready-to-use implementation and template)
\

## Supported devices and boards

- STM32C5 series.
\

## Known Limitations

- Configuration and code generation using STM32CubeMX tool will be available soon.
\

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V9.60.3 + ST-LINK
- MDK-ARM Keil uVision V5.42
- STM32CubeIDE for Visual Studio Code (GCC13 compiler)
\

## Backward compatibility

None.
\

## Dependencies

None.
\

</div>
:::


\

\

:::
:::

<footer class="sticky">
For complete documentation on STM32 Microcontrollers </mark> ,
visit: [[www.st.com/stm32](http://www.st.com/stm32)]{style="font-color: blue;"}

This release note uses up to date web standards and, for this reason, should not
be opened with Internet Explorer but preferably with popular browsers such as
Google Chrome, Mozilla Firefox, Opera or Microsoft Edge.
</footer>
