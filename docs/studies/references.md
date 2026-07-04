# Fanatec Ecosystem Source Register

> Reviewed: 2026-07-02  
> Purpose: classify ecosystem-level sources before their claims are reused in technical or customer-facing documents

## Source Register

| Source | Type | Reviewed State | Approved Use | Do Not Use As |
|---|---|---|---|---|
| [Fanatec Ecosystem Diagram](https://help.fanatec.com/hc/de/articles/43786297099281-Fanatec-Ecosystem-Diagramm) | Official support visual | URL verified; page content not machine-extractable in this audit | Official visual entry point for ecosystem relationships | Evidence for unseen labels, protocols, or electrical behavior |
| [OC Racing: Fanatec Ecosystem Explained for Dummies in 2026](https://ocracing.com/guides/fanatec-ecosystem-explained-for-dummies/) | Community buyer guide, published 2026-06-07 | Current enough to reflect Podium DD and 15/18 Nm ClubSport update | Beginner mental model, product-tier overview, discovery | Technical specification, safety requirement, exhaustive compatibility matrix |
| [Sim Racing Setups: Fanatec Ecosystem Explained](https://simracingsetup.com/product-guides/fanatec-ecosystem-explained/) | Community buyer guide, updated 2026-01-13 | Partly stale by July 2026 | Modular buying model, Ready2Race explanation, legacy context | Current torque/flagship claims, protocol facts, authoritative compatibility |

## Current Official Cross-Checks

| Topic | Official Source | Verified Conclusion |
|---|---|---|
| Wheel-base tiers | [Wheel Bases FAQ](https://help.fanatec.com/hc/en-us/articles/43766204938257-Wheel-Bases-A-FAQ) | Current tiers are CSL, ClubSport, and Podium; exact product compatibility remains model-specific. |
| Xbox licensing | [Understanding Fanatec Xbox Compatibility](https://www.fanatec.com/us/en/explorer/products/racing-wheels-wheel-bases/understanding-fanatec-xbox-compatibility/) | Xbox support comes from an Xbox-licensed steering wheel or hub. |
| PlayStation licensing | [Fanatec platform compatibility](https://www.fanatec.com/us-en/platforms) | PlayStation support comes from a PlayStation-licensed wheel base. |
| Current QR generation | [Steering Wheel FAQ](https://help.fanatec.com/hc/en-us/articles/43802514108433-Steering-Wheel-FAQ) | Fanatec-store wheels/bases are QR2 by default as of 2026-02-16; QR1 is discontinued. |
| QR conversion | [QR2 conversion guidance](https://help.fanatec.com/hc/en-us/articles/30011253510289-Which-products-can-be-converted-to-QR2) | Both Base-Side and Wheel-Side must use the same QR generation; conversion support is model-specific. |
| ClubSport torque | [More Torque, Same Hardware](https://www.fanatec.com/us/en/explorer/products/racing-wheels-wheel-bases/more-torque-same-hardware/) | Current firmware raises ClubSport DD/DD+ holding torque from 12/15 Nm to 15/18 Nm. |
| Console accessories | [Wheel Bases FAQ](https://help.fanatec.com/hc/en-us/articles/43766204938257-Wheel-Bases-A-FAQ) | Fanatec accessories must route through the wheel base on consoles; standalone USB is a PC path. |
| CSL Pedals USB | [CSL Pedals direct USB guidance](https://help.fanatec.com/hc/en-us/articles/30312127196945-How-can-I-connect-CSL-Pedals-directly-to-a-PC-via-USB) | Base CSL Pedals require a Load Cell Kit or ClubSport USB Adapter for standalone PC USB. |
| Shifter ports | [Shifter-port guidance](https://help.fanatec.com/hc/en-us/articles/45597346898449-Which-shifter-port-should-I-use-on-my-Fanatec-wheel-base) | Shifter 1 supports ClubSport Shifter H-pattern/SQ; Shifter 2 supports sequential/static paddles. |

## Known Currency Conflict

The Sim Racing Setups guide reports ClubSport DD/DD+ at 12/15 Nm and DD2 as the highest current base. These statements describe an earlier 2026 state. For July 2026 documentation, use Fanatec's 15/18 Nm firmware update and current Podium DD material instead.

## Reuse Rules

- Cite official sources for compatibility, torque, QR, firmware, safety, and connection behavior.
- Cite community guides only for dated explanations, buyer mental models, and discovery.
- Do not infer electrical buses, authentication mechanisms, or protocol details from an ecosystem diagram or buyer guide.
- Keep prices and availability out of evergreen architecture docs.

## Related Documents

- [Customer glossary](./glossary.md)
- [Ecosystem research](./sim_racing_research.md)
- [Beginner knowledge base](./sim_racing_knowledge_base_en.md)
- [Developer tools and references](./tools.md)

## Unresolved Questions

- None.
