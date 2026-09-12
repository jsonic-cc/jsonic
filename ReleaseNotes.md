# Release notes

## RFC 8259 conformance hardening

- Accept and preserve duplicate object members, which RFC 8259 permits
  syntactically while warning about interoperability.
- Reject malformed, overlong, surrogate and out-of-range unescaped UTF-8.
- Enforce a 512-level nesting limit so pathological input fails cleanly.
- Add focused regression coverage for all three failure classes.

## Initial standalone checkpoint

Jsonic++ begins as the standalone home of the dependency-free JSON implementation already used by Nift and Minify++. The initial checkpoint preserves the established parser API and behavior, adds standalone smoke/adversarial/sanitizer targets, and makes synchronization with embedded copies explicit.
