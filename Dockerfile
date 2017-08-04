FROM i386/debian:stretch-slim

RUN useradd -ms /bin/bash wineuser

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        procps \
        wine \
        xvfb \
    && rm -rf /var/lib/apt/lists/*

USER wineuser
ENV WINEPREFIX /home/wineuser/.wine
ENV WINEARCH win32
# Silence all the "fixme: blah blah blah" messages from wine
ENV WINEDEBUG fixme-all
ENV DISPLAY :0
WORKDIR /home/wineuser

RUN wineboot --init \
    && while pgrep wineserver >/dev/null; do echo "Waiting for wineserver"; sleep 1; done

COPY .asoundrc *.dic *.dll winetalk.sh winetalk.sh.so scripts/run.sh ./

ENTRYPOINT ["./run.sh"]
