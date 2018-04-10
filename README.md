# buff.filter

> A Max external to filter a buffer through a bank of biquads (offline filtering).

Author: Yves Candau

This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with this file, you can obtain one at [http://mozilla.org/MPL/2.0/](http://mozilla.org/MPL/2.0/).

## Notes

A biquad filter can be defined by its coefficients: a0, a1, a2, b1, b2, and implements the equation:

```Y(n) = a0 * X(n) + a1 * X(n - 1) + a2 * X(n - 2) - b1 * Y(n - 1) - b2 * Y(n - 2)```

Alternatively, a number of specific biquad filters can be defined by their types and three parameters:

- type: lowpass | highpass | bandpass | bandstop | peaknotch | lowshelf | highshelf | resonant | allpass
- frequency
- gain
- Q

For a given type and set of parameters, the coefficients are samplerate dependent. They should thus be recalculated each time the samplerate changes.

## Inlets

0. All purpose inlet

## Outlets

0. General messages (unused at the moment)
1. Bang on completion

## Arguments

None

## Attributes

None

## Messages

- ```eq <buffer name (symbol)>```

Apply the bank of biquad filters to a buffer.

- ```set_coeff <a0 (float), a1 (float), a2 (float), b1 (float), ab2 (float)> {1, }```

Set a bank of biquad filters, determined by their coefficients. The message should consist of a series of quintuplets holding the float coefficients.

- ```set_param <type (symbol), frequency (float), gain (float), Q (float)> {1, }```

Set a bank of biquad filters, determined by their types and parameters. The possible types are listed above in the notes. The message should consist of a series of quadruplets holding the type and three parameters.

- ```mute: <0 | 1>```

Mute or unmute error reporting.

- ```post biquads```

Post information on the bank of biquad filters, including their coefficients, and if available, their types and parameters.
