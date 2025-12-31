# Contribution Guidelines (Summary)

Source: `CONTRIBUTING.md`

## Commit Policy

- Keep commits small and atomic
- Do not mix formatting and code changes
- Do not mix coding style fixes with code changes
- For larger changes, use KDE GitLab or contact maintainer
- i18n and documentation fixes can be committed directly

## Coding Style

- Follows KDELibs style with Spectacle-specific exceptions
- Access modifiers align with member declarations
- Modifier order: public, signals, public slots, protected slots, protected, private slots, private
- Member variables use `mCamelCase` (not `m_camelCase`)
- Source files are mixed case matching the class name
