#!/bin/bash

REPO_NAME="${GITHUB_REPOSITORY#*/}"

if jq -e '.nodes | length > 0' security-report.json >/dev/null 2>&1; then

    VULNS_MD=$(jq -r '
    .nodes |
    sort_by(.createdAt) | reverse |
    sort_by(.securityVulnerability.severity | {CRITICAL: 0, HIGH: 1, MODERATE: 2, LOW: 3}[.]) |
    "| Date | Package | Sévérité | " +
    "Version Vulnérable | ID GHSA | Description |",
    "| --- | --- | --- | --- | --- | --- |",
    (.[] |
      "| \(.createdAt[0:10]) | " +
      "`\(.securityVulnerability.package.name)` | " +
      "**\(.securityVulnerability.severity)** | " +
      "`\(.securityVulnerability.vulnerableVersionRange)` | " +
      "\(.securityVulnerability.advisory.ghsaId) | " +
      "\(.securityVulnerability.advisory.summary | gsub("\\|"; "&#124;")) |"
    )
  ' security-report.json)

    cat <<EOF >issue-body.md
## Vulnérabilités détectées dans \`$REPO_NAME\`

$VULNS_MD

---
*Rapport généré automatiquement le $(date +'%Y-%m-%d')*
EOF

    gh label create security \
        --color "FF0000" \
        --description "Security vulnerabilities" \
        2>/dev/null || true

    gh issue create \
        --title "Security Report - $REPO_NAME - $(date +'%Y-%m-%d')" \
        --body-file issue-body.md \
        --label "security"
else
    echo "Aucune vulnérabilité ouverte détectée."
fi
