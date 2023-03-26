#ifndef _KIRSCH_H
#define _KIRSCH_H

void kirsch_operator_avx
(
  unsigned char *data_out
, unsigned char *data_in
, unsigned height
, unsigned width
);

void kirsch_operator_sse
(
  unsigned char *data_out
, unsigned char *data_in
, unsigned height
, unsigned width
);

#endif	