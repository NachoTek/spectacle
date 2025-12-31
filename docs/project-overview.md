# Project Overview

## Purpose

Spectacle is a KDE/Qt desktop screenshot and screen recording utility. It integrates with KDE infrastructure and can also be used in non-KDE X11 environments.

## Executive Summary

Single-part Qt/KDE desktop application. Core logic lives in `src/`, with platform capture backends in `src/Platforms/` and the UI layer in `src/Gui/` (QML + widgets). Integration is primarily via DBus and KDE configuration.

## Tech Stack

- Language: C++
- Frameworks: Qt 6, KDE Frameworks 6
- Build: CMake
- Optional: Tesseract OCR, KF6DocTools

## Architecture Type

Desktop app with platform-specific capture backends and DBus integrations.

## Repository Structure

Monolith (single cohesive repository).

## Documentation Links

- Architecture: `docs/architecture.md`
- Source Tree: `docs/source-tree-analysis.md`
- UI Components: `docs/ui-component-inventory-root.md`
- Development: `docs/development-instructions.md`
- Deployment: `docs/deployment-configuration.md`
- Contribution: `docs/contribution-guidelines.md`