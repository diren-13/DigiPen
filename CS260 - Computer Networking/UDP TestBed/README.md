# UDP TestBed

Testing abstraction for using UDP as part of a networked application.


<h3>Architecture</h3>

```mermaid
classDiagram
  direction BT

  class AddressInfo {
    - ipv4Address
    - port
  }

  class Socket {
    <<Open>>
    # type
    # id
    # handle
    # isBlocking

    Create()
    IsInvalid()
    Close()

    AddressInfo CreateAddress(ipv4Address, port)
    Status GetErrorStatus()
  }

  class Status {
    <<Enumeration>>
    Done
    NotReady
    Error
  }

  class Type {
    <<Enumeration>>
    UDP
    TCP
  }

  class UDPSocket {
    - buffer
    - bufferSize

    Status Bind(ipv4Address, port)
    Unbind()
    Status Send(*data, dataSize, targetAddr, targetPort)
    Status Receive(*data, dataSize, targetAddr, targetPort)
  }


  %% Define Relationships
  Socket      --    AddressInfo
  Socket      --    Status
  Socket      *--   Type
  UDPSocket   <|--  Socket

```