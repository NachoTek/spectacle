# Deployment Configuration

## CI/CD

CI is configured via `.gitlab-ci.yml` and includes KDE CI templates:

- /gitlab-templates/linux-qt6.yml
- /gitlab-templates/freebsd-qt6.yml
- #     - /gitlab-templates/flatpak.yml
- /gitlab-templates/xml-lint.yml
- /gitlab-templates/yaml-lint.yml
- /gitlab-templates/qml-lint.yml
- /gitlab-templates/linux-qt6-next.yml
- /gitlab-templates/documentation.yml

## Deployment

No Docker, Kubernetes, or dedicated deployment scripts detected in the repository. Deployment is handled via KDE infrastructure and release service per project conventions.
