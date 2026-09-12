# Jsonic++ decisions

- Keep the project header-only and dependency-free.
- Keep the public header named `json.h`.
- Preserve the existing `json` namespace and `json::Document` API during extraction.
- Accept duplicate object keys and preserve them in source order because RFC
  8259 permits them syntactically; applications should avoid relying on their
  interpretation.
- Require well-formed UTF-8 in unescaped string content.
- Reject nesting deeper than 512 arrays/objects before stack exhaustion.
- Standalone Jsonic++ owns parser semantics; Nift and Minify++ vendor synchronized copies.
- Generalize only after concrete parser contracts justify the added surface.
