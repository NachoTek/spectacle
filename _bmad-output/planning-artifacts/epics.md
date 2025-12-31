---
stepsCompleted: [1, 2, 3, 4]
inputDocuments:
  - C:\projects\spectacle\_bmad-output\planning-artifacts\prd.md
  - C:\projects\spectacle\_bmad-output\planning-artifacts\architecture.md
  - C:\projects\spectacle\_bmad-output\planning-artifacts\ux-design-specification.md
---

# spectacle - Epic Breakdown

## Overview

This document provides the complete epic and story breakdown for spectacle, decomposing the requirements from the PRD, UX Design if it exists, and Architecture requirements into implementable stories.

## Requirements Inventory

### Functional Requirements

FR1: User can initiate a screenshot capture session from within the application.
FR2: User can capture the full screen.
FR3: User can capture a user-defined region of the screen.
FR4: User can capture an active window.
FR5: User can set a delayed capture and execute it after the delay.
FR6: User can cancel an in-progress capture session.
FR7: User can open a pre-capture overlay prior to finalizing a screenshot.
FR8: User can draw freehand annotations on the pre-capture overlay.
FR9: User can add arrow annotations on the pre-capture overlay.
FR10: User can add box/rectangle annotations on the pre-capture overlay.
FR11: User can add circle/ellipse annotations on the pre-capture overlay.
FR12: User can adjust the capture selection area while the overlay is active.
FR13: User can keep the capture session active while adjusting on-screen content before finalizing the capture.
FR14: User can annotate a captured image after capture.
FR15: User can add freehand, arrow, box/rectangle, and circle/ellipse annotations after capture.
FR16: User can start a screen recording session.
FR17: User can stop a screen recording session.
FR18: User can save a completed recording to disk.
FR19: User can perform OCR on a captured image.
FR20: User can access the extracted text output from OCR.
FR21: User can save captured images to disk.
FR22: User can set a default save location for captures.
FR23: User can set a default image format for captures.
FR24: User can trigger Spectacle using the global Print Screen hotkey.
FR25: User can customize the capture hotkey to resolve conflicts.
FR26: User can install Spectacle on Windows 11 via a standard installer (EXE/MSI).
FR27: User can install Spectacle on Windows 11 via `winget`.
FR28: User can receive update notifications.
FR29: User can choose whether to install an available update now or defer it.
FR30: User can generate a bug report with diagnostic information for capture or overlay issues.
FR31: User can perform capture, annotation, recording, and OCR workflows without network connectivity.

### NonFunctional Requirements

NFR1: Capture result is available within <1 second from initiation.
NFR2: Recording begins within <1 second from activation.
NFR3: 100% crash-free in acceptance testing.
NFR4: 100% Print Screen hotkey reliability in acceptance testing.
NFR5: No telemetry data is collected or transmitted.

### Additional Requirements

- Windows 11 only; maintain parity with Linux Spectacle workflows.
- UI stack remains Qt Quick/QML + widgets; use Windows 11 Fluent styling, Segoe UI Variable, system accent, DPI-aware scaling.
- Two entry modes: Print Screen overlay flow and app window quick actions; both converge into the same annotation flow.
- Overlay behavior: crosshair + hover highlight; click to select target bounds; resize with handles; Enter confirms capture.
- Floating quick tray docks outside selection when possible; if inside, avoid active annotation area and cursor path.
- No toasts/notifications; use inline status feedback only.
- Clipboard updates on annotation changes (pre and post capture).
- Helper process always-on with Registry Run key; IPC via Win32 named pipes with JSON envelope.
- Capture backend: Windows Graphics Capture (WGC).
- Recording backend: WGC + Media Foundation pipeline.
- OCR: Tesseract primary, Windows OCR fallback (offline).
- Packaging: NSIS installer + winget manifest; Windows CI in GitHub Actions.
- Accessibility: WCAG 2.1 AA; full keyboard operation; high-contrast support.

### FR Coverage Map

### FR Coverage Map

FR1: Epic 1 - Start capture from app
FR2: Epic 1 - Full screen capture
FR3: Epic 1 - Region capture
FR4: Epic 1 - Active window capture
FR5: Epic 1 - Delayed capture
FR6: Epic 1 - Cancel capture
FR7: Epic 1 - Pre-capture overlay
FR8: Epic 1 - Pre-capture free draw
FR9: Epic 1 - Pre-capture arrow
FR10: Epic 1 - Pre-capture box
FR11: Epic 1 - Pre-capture circle
FR12: Epic 1 - Adjust selection in overlay
FR13: Epic 1 - Keep overlay active while adjusting UI
FR14: Epic 1 - Post-capture annotation
FR15: Epic 1 - Post-capture annotation tools
FR16: Epic 2 - Start recording
FR17: Epic 2 - Stop recording
FR18: Epic 2 - Save recording
FR19: Epic 3 - Perform OCR
FR20: Epic 3 - Access OCR output
FR21: Epic 1 - Save captures to disk
FR22: Epic 1 - Default save location
FR23: Epic 1 - Default image format
FR24: Epic 1 - Global Print Screen hotkey
FR25: Epic 4 - Customize hotkey
FR26: Epic 5 - Install via EXE/MSI
FR27: Epic 5 - Install via winget
FR28: Epic 5 - Update notifications
FR29: Epic 5 - Defer updates
FR30: Epic 4 - Bug report with diagnostics
FR31: Epic 4 - Offline operation


## Epic List

## Epic List

### Epic 1: Screenshot Capture & Annotation Flow
Enable users to capture screenshots with pre/post annotation, save to disk/clipboard, and complete the core workflow quickly.
**FRs covered:** FR1, FR2, FR3, FR4, FR5, FR6, FR7, FR8, FR9, FR10, FR11, FR12, FR13, FR14, FR15, FR21, FR22, FR23, FR24

### Epic 2: Screen Recording Workflow
Enable users to start/stop recordings and save output reliably.
**FRs covered:** FR16, FR17, FR18

### Epic 3: OCR Text Extraction
Enable users to extract and access text from captures offline.
**FRs covered:** FR19, FR20

### Epic 4: Hotkeys, Reliability & Diagnostics
Enable hotkey customization, offline operation guarantees, and diagnostics/bug reporting.
**FRs covered:** FR25, FR30, FR31

### Epic 5: Installation & Updates
Enable install via EXE/MSI + winget, and update notifications/deferral.
**FRs covered:** FR26, FR27, FR28, FR29



## Epic 1: Screenshot Capture & Annotation Flow

Enable users to capture screenshots with pre/post annotation, save to disk/clipboard, and complete the core workflow quickly.

### Story 1.1: Print Screen Region Capture (Overlay + Confirm/Cancel)

As a Windows 11 user,
I want to press Print Screen to open a capture overlay and select a region,
So that I can quickly capture part of the screen and confirm or cancel.

**Acceptance Criteria:**

**Given** Spectacle is running and the Print Screen hotkey is enabled
**When** I press Print Screen
**Then** a full-screen overlay appears within 1 second and shows a crosshair cursor

**Given** the overlay is active
**When** I click-drag to define a rectangular region
**Then** the selection box is created and can be resized via grab handles

**Given** a region is selected
**When** I press Enter
**Then** the capture is finalized and the image is committed to the clipboard (and autosaved if enabled)

**Given** the overlay is active
**When** I press Escape or click Cancel
**Then** the capture is aborted and no image is committed

### Story 1.2: Selection Targeting + Refinement

As a Windows 11 user,
I want the overlay to highlight UI targets and let me refine the selection,
So that I can quickly capture precise window or menu bounds and adjust my selection.

**Acceptance Criteria:**

**Given** the overlay is active
**When** I move the cursor over windows, dialogs, or menus
**Then** the hovered target is highlighted to indicate it can be selected

**Given** a target is highlighted
**When** I click it
**Then** the selection bounds snap to that target's size

**Given** a selection exists
**When** I drag a grab handle
**Then** the selection resizes accordingly

**Given** the overlay is active
**When** I adjust underlying UI (move or resize a window)
**Then** the overlay remains active and the selection can be updated afterward

### Story 1.3: Pre-Capture Annotation Tools

As a Windows 11 user,
I want to annotate before capture using drawing tools,
So that I can add callouts and markup before finalizing the screenshot.

**Acceptance Criteria:**

**Given** the overlay is active and a selection exists
**When** I choose free draw, arrow, box, or circle
**Then** I can draw that annotation within the selection bounds

**Given** I add an annotation
**When** I continue editing
**Then** the annotation stays editable until capture is confirmed

**Given** I change annotation color or stroke size
**When** I draw
**Then** the new style is applied to subsequent annotations

**Given** I press Enter
**When** annotations are present
**Then** the annotations are included in the captured image

### Story 1.4: Post-Capture Annotation View

As a Windows 11 user,
I want to edit annotations after capture,
So that I can refine or add markup before saving or sharing.

**Acceptance Criteria:**

**Given** I have finalized a capture
**When** the post-capture view opens
**Then** I can annotate using the same tools as the overlay

**Given** I edit the image after capture
**When** I add or modify annotations
**Then** the clipboard is updated to reflect the current annotations

**Given** I save or copy the image
**When** post-capture annotations are present
**Then** the saved/copied image includes the latest annotations

### Story 1.5: Saving & Defaults

As a Windows 11 user,
I want captures to save to disk and clipboard based on my defaults,
So that I don't have to reconfigure save options each time.

**Acceptance Criteria:**

**Given** I have a completed capture
**When** autosave is enabled
**Then** the image is saved to the configured default location in the default format

**Given** I change the default save location or format
**When** I capture again
**Then** the new defaults are used

**Given** autosave is disabled
**When** I complete a capture
**Then** I can manually save the image to disk

**Given** any capture is completed
**When** it is finalized
**Then** the clipboard is updated with the final image

### Story 1.6: Additional Capture Modes

As a Windows 11 user,
I want full screen, active window, and delayed capture options,
So that I can capture the right content without manual region selection each time.

**Acceptance Criteria:**

**Given** I choose Full Screen capture
**When** I confirm the action
**Then** the current display (or all displays if selected) is captured and sent to the annotation flow

**Given** I choose Active Window capture
**When** I confirm the action
**Then** the currently active window is captured and sent to the annotation flow

**Given** I choose Delayed capture
**When** I set a delay and confirm
**Then** the capture starts after the delay and proceeds to annotation

### Story 1.7: App Window Quick Actions

As a Windows 11 user,
I want to open the app and choose quick capture actions,
So that I can start a capture without using the hotkey.

**Acceptance Criteria:**

**Given** I open the Spectacle app
**When** the main window appears
**Then** I see quick actions for Display, All Displays, Active Window, and Rectangular Region

**Given** I select Display
**When** I hover and click a monitor
**Then** that display is captured and sent to the annotation flow

**Given** I select All Displays
**When** I confirm
**Then** all monitors are captured and sent to the annotation flow

**Given** I select Active Window or Rectangular Region
**When** I confirm
**Then** the corresponding capture is performed and sent to the annotation flow

## Epic 2: Screen Recording Workflow

Enable users to start/stop recordings and save output reliably.

### Story 2.1: Start Recording from App/Overlay

As a Windows 11 user,
I want to start a screen recording quickly from Spectacle,
So that I can capture video without delay.

**Acceptance Criteria:**

**Given** Spectacle is open (or I use the recording entry point)
**When** I start a recording
**Then** recording begins within 1 second

**Given** recording has started
**When** I see the recording state
**Then** I get clear in-app status that recording is active

**Given** I choose a specific capture target (display, window, or region)
**When** I start recording
**Then** the recording is limited to that target

### Story 2.2: Stop Recording and Save Output

As a Windows 11 user,
I want to stop a recording and save it reliably,
So that I can use the recording without re-capturing.

**Acceptance Criteria:**

**Given** a recording is active
**When** I stop the recording
**Then** the recording ends cleanly and the output file is saved

**Given** I have a default save location and format
**When** the recording is saved
**Then** the file is stored using those defaults

**Given** a recording is saved
**When** saving completes
**Then** I see a clear in-app confirmation that the recording is available

## Epic 3: OCR Text Extraction

Enable users to extract and access text from captures offline.

### Story 3.1: Run OCR on Captured Image

As a Windows 11 user,
I want to run OCR on a captured image,
So that I can extract text from screenshots offline.

**Acceptance Criteria:**

**Given** I have a captured image
**When** I select the OCR action
**Then** the OCR process runs using the offline engine

**Given** OCR processing completes
**When** text is extracted
**Then** the output is available for viewing or copying

**Given** OCR is not available (missing engine or language data)
**When** I attempt OCR
**Then** I see an in-app message explaining what is missing

### Story 3.2: Access OCR Output

As a Windows 11 user,
I want to access OCR results after extraction,
So that I can copy or use the text immediately.

**Acceptance Criteria:**

**Given** OCR has completed
**When** the results are shown
**Then** I can copy the extracted text

**Given** I reopen the OCR results
**When** I view the text
**Then** the content matches the latest OCR run for that image

## Epic 4: Hotkeys, Reliability & Diagnostics

Enable hotkey customization, offline operation guarantees, and diagnostics/bug reporting.

### Story 4.1: Always-On Hotkey Reliability (Helper + IPC)

As a Windows 11 user,
I want Print Screen to trigger capture reliably even when the main app is closed,
So that I can always start a capture instantly.

**Acceptance Criteria:**

**Given** the helper process is installed
**When** I sign in to Windows
**Then** the helper starts automatically in the background

**Given** the helper is running
**When** I press Print Screen
**Then** the capture overlay appears within 1 second (or the app window opens if needed)

**Given** the IPC connection to the app is disrupted
**When** I press Print Screen
**Then** the helper retries the connection and the capture request completes once the app is available

### Story 4.2: Hotkey Customization

As a Windows 11 user,
I want to change the capture hotkey,
So that I can avoid conflicts with other tools.

**Acceptance Criteria:**

**Given** I am in Settings > Hotkeys
**When** I set a new hotkey and save
**Then** the new hotkey triggers the capture overlay

**Given** a hotkey is already in use by another app
**When** I try to set it in Spectacle
**Then** I see an in-app warning and can choose a different hotkey

**Given** I change the hotkey
**When** I press the old hotkey
**Then** Spectacle does not start a capture

### Story 4.3: Diagnostics / Bug Report (Local)

As a Windows 11 user,
I want to generate a local diagnostics report,
So that I can share capture issues without sending data online.

**Acceptance Criteria:**

**Given** I open Diagnostics from the app
**When** I generate a report
**Then** a local file is created with logs and system context

**Given** I save a diagnostics report
**When** I open the file location
**Then** the report is available without any network connection

**Given** a capture or overlay failure occurs
**When** I open Diagnostics
**Then** I can include recent capture logs in the report

### Story 4.4: Offline-First Guarantees

As a Windows 11 user,
I want capture, annotation, recording, and OCR to work offline,
So that I can use Spectacle without network connectivity.

**Acceptance Criteria:**

**Given** the device is offline
**When** I capture or annotate an image
**Then** the workflow completes without errors

**Given** the device is offline
**When** I start and stop a recording
**Then** the recording is saved locally as expected

**Given** the device is offline
**When** I run OCR
**Then** text extraction completes using the offline engine

## Epic 5: Installation & Updates

Enable install via EXE/MSI + winget, and update notifications/deferral.

### Story 5.1: NSIS Installer (EXE/MSI)

As a Windows 11 user,
I want to install Spectacle using a standard installer,
So that setup is quick and familiar.

**Acceptance Criteria:**

**Given** I run the installer
**When** installation completes
**Then** the app and helper are installed and can be launched

**Given** I complete installation
**When** I sign in again
**Then** the helper starts automatically via the Run key

**Given** I uninstall Spectacle
**When** uninstallation completes
**Then** the app, helper, and Run key entry are removed

### Story 5.2: Winget Installation

As a Windows 11 user,
I want to install Spectacle using winget,
So that I can automate installs or use my standard tooling.

**Acceptance Criteria:**

**Given** a winget manifest is published
**When** I run `winget install spectacle`
**Then** Spectacle installs successfully

**Given** I already have Spectacle installed
**When** I run `winget upgrade`
**Then** Spectacle updates to the latest version

### Story 5.3: Update Notifications & Deferral

As a Windows 11 user,
I want to know when updates are available and choose when to install them,
So that I can control disruptions.

**Acceptance Criteria:**

**Given** an update is available
**When** I open the app
**Then** I see an in-app update prompt with options to install now or later

**Given** I choose to defer an update
**When** I reopen the app
**Then** I can continue using Spectacle and the update remains available
